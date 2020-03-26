/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ClassBasedHeapAnalyzer.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/lang/ClassInfo.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::ArrayList;
using coherence::util::Arrays;
using coherence::util::AtomicCounter;
using coherence::util::Comparator;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::StringHelper;

COH_REGISTER_TYPED_CLASS(ClassBasedHeapAnalyzer);


// ----- file local helpers -------------------------------------------------

COH_OPEN_NAMESPACE_ANON(ClassBasedHeapAnalyzer)

// ----- inner class: ClassBasedComparator ------------------------------

/**
* ClassBasedComparator compares Map::Entries containing instance counts.
*/
class COH_EXPORT ClassBasedComparator
    : public class_spec<ClassBasedComparator,
        extends<Object>,
        implements<Comparator> >
    {
    friend class factory<ClassBasedComparator>;

    // ----- factory methods ----------------------------------------

    public:
        static Handle getInstance()
            {
            static FinalHandle<ClassBasedComparator>
                h(System::common(), create());
            return h;
            }

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new ClassBasedComparator.
        */
        ClassBasedComparator()
            {
            }

    // ----- Comparator interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const
            {
            Map::Entry::View vE1 = cast<Map::Entry::View>(v1);
            Map::Entry::View vE2 = cast<Map::Entry::View>(v2);

            // sort first by count, then by class name
            int32_t nOrder = cast<Comparable::View>(vE1->getValue())->
                    compareTo(vE2->getValue());

            return nOrder == 0 ? cast<Comparable::View>(vE1->getKey())->
                    compareTo(vE2->getKey()) : nOrder;
            }
    };
    COH_STATIC_INIT(ClassBasedComparator::getInstance());

COH_CLOSE_NAMESPACE_ANON

namespace
    {
    /**
    * Compute the delta between two sets of ClassInfo maps.
    *
    * @param vMapA  the first info map
    * @param vMapB  the second info map
    *
    * @return the result of vMapA - vMapB
    */
    template<class A, class B>
    Map::View computeDelta(Map::View vMapA, Map::View vMapB)
        {
        // avoid having the cost of the delta object show up in the delta, as if
        // we are comparing to just before taking the delta; create no managed
        // objects untill we've recorded the current values into non-managed storage
        size32_t       cEntries = vMapB->size();
        Object::View*  avKey    = new Object::View[cEntries];
        int64_t*       aco      = new int64_t     [cEntries];
        int64_t*       acb      = new int64_t     [cEntries];
        int64_t*       aca      = new int64_t     [cEntries];

        try
            {
            size32_t ofClass = 0;
            for (Iterator::Handle hIter = vMapB->entrySet()->iterator();
                    ofClass < cEntries && hIter->hasNext(); ++ofClass)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                Object::View     vKey   = vEntry->getKey();
                typename A::View vStatA = cast<typename A::View>(vMapA->get(vKey));
                typename B::View vStatB = cast<typename B::View>(vMapB->get(vKey));
                int64_t          coA    = NULL == vStatA ? 0 : vStatA->getObjectCount();
                int64_t          coB    = vStatB->getObjectCount();
                int64_t          cbA    = NULL == vStatA ? 0 : vStatA->getByteCount();
                int64_t          cbB    = vStatB->getByteCount();
                int64_t          caA    = NULL == vStatA ? 0 : vStatA->getAllocationCount();
                int64_t          caB    = vStatB->getAllocationCount();

                avKey[ofClass] = vKey;
                aco  [ofClass] = coA - coB;
                acb  [ofClass] = cbA - cbB;
                aca  [ofClass] = caA - caB;
                }

            // now we can create managed objects
            Map::Handle hMapDelta = HashMap::create();
            for (size32_t i = 0; i < ofClass; ++i)
                {
                int64_t co = aco[i];
                int64_t ca = aca[i];
                if (co || ca)
                    {
                    hMapDelta->put(avKey[i], ClassBasedHeapAnalyzer::
                            ClassStats::create(co, acb[i], ca));
                    }
                }

            // done creating delta; cleanup
            delete[] avKey;
            avKey = NULL;
            delete[] aco;
            aco = NULL;
            delete[] acb;
            acb = NULL;
            delete[] aca;
            aca = NULL;

            return hMapDelta;
            }
        catch (...)
            {
            if (avKey)
                {
                delete[] avKey;
                }
            if (aco)
                {
                delete[] aco;
                }
            if (acb)
                {
                delete[] acb;
                }
            throw;
            }
        }
    }


// ----- constructor --------------------------------------------------------

ClassBasedHeapAnalyzer::ClassBasedHeapAnalyzer(bool fShowAllocations)
    : m_fShowAllocations(fShowAllocations)
    {
    }


// ----- HeapAnalyzer interface ---------------------------------------------

HeapAnalyzer::Snapshot::View ClassBasedHeapAnalyzer::capture() const
    {
    System::gc();

    Map::View        vMapSrc = ClassInfo::getClassInfoMap();
    Map::Handle      hMapDes = HashMap::create();
    Snapshot::Handle hSnap   = Snapshot::create(hMapDes, m_fShowAllocations);
    // We want to ensure that the objects created by making the Snapshot are
    // included. If they were not, then they wouldn't show up until the next
    // snapshot which would be confusing.

    // first create all the map entries, and avoid cloneing strings
    hMapDes->putAll(vMapSrc);

    // now create each ClassStat
    for (Iterator::Handle hIter = hMapDes->entrySet()->iterator();
            hIter->hasNext(); )
        {
        Map::Entry::Handle hEntry = cast<Map::Entry::Handle>(hIter->next());
        ClassInfo::View    vInfo  = cast<ClassInfo::View>(hEntry->getValue());

        hEntry->setValue(ClassStats::create(vInfo->getObjectCount(),
                vInfo->getByteCount(), vInfo->getAllocationCount()));
        }

    // finally update the ClassStat with the latest value, thus reflecting
    // all of the above values
    String::View    vsKey = Class::getTypeName(typeid(ClassStats));
    ClassInfo::View vInfo = cast<ClassInfo::View>(vMapSrc->get(vsKey));
    hMapDes->put(vsKey, ClassStats::create(vInfo->getObjectCount(),
            vInfo->getByteCount(), vInfo->getAllocationCount()));

    return hSnap;
    }

HeapAnalyzer::Snapshot::View
ClassBasedHeapAnalyzer::delta(HeapAnalyzer::Snapshot::View vSnap) const
    {
    return Snapshot::create(
            computeDelta<ClassInfo, ClassStats>(ClassInfo::getClassInfoMap(),
                    cast<Snapshot::View>(vSnap)->getStatsMap()),
            m_fShowAllocations);
    }

int64_t ClassBasedHeapAnalyzer::getObjectCount() const
    {
    System::gc();

    int64_t cTotal = 0;
    for (Iterator::Handle hIter = ClassInfo::getClassInfoMap()->entrySet()->iterator();
            hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        ClassInfo::View  vInfo  = cast<ClassInfo::View>(vEntry->getValue());

        cTotal += vInfo->getObjectCount();
        }
    return cTotal;
    }

int64_t ClassBasedHeapAnalyzer::getImmortalCount() const
    {
    return m_cImmortals.get();
    }

void ClassBasedHeapAnalyzer::safeRegisterObject(const Object& o)
    {
    if (System::isCommonMonitorInitialized())
        {
        ClassInfo* pInfo = ClassInfo::findByType(typeid(o));
        if (pInfo != NULL)
            {
            pInfo->adjustObjectCount(/*fIncrement*/ true);
            pInfo->adjustByteCount(o.sizeOf());
            pInfo->incrementAllocationCount();
            }
        // else only occurs during static destruction
        }
    // else; static initialization
    }

void ClassBasedHeapAnalyzer::safeUnregisterObject(const Object& o)
    {
    if (System::isCommonMonitorInitialized())
        {
        ClassInfo* pInfo = ClassInfo::findByType(typeid(o));
        if (pInfo != NULL)
            {
            pInfo->adjustObjectCount(/*fIncrement*/ false);
            pInfo->adjustByteCount(-int64_t(o.sizeOf()));
            }
        // else only occurs during static destruction
        }
    // else; static initialization
    }

void ClassBasedHeapAnalyzer::registerObject(const Object& o)
    {
    ClassInfo* pInfo = ClassInfo::findByType(typeid(o),
            /*fEnsure*/ false);
    if (NULL == pInfo)
        {
        // registering would create Objects, take safe route; this only
        // happens the first time we see an object of this type
        AbstractHeapAnalyzer::registerObject(o);
        }
    else
        {
        pInfo->adjustObjectCount(/*fIncrement*/ true);
        pInfo->adjustByteCount(o.sizeOf());
        pInfo->incrementAllocationCount();
        }
    }

void ClassBasedHeapAnalyzer::unregisterObject(const Object& o)
    {
    ClassInfo* pInfo = ClassInfo::findByType(typeid(o),
            /*fEnsure*/ false);
    if (NULL == pInfo)
        {
        // unregistering would create Objects, take safe route; this only
        // happens the first time we see an object of this type
        AbstractHeapAnalyzer::unregisterObject(o);
        }
    else
        {
        pInfo->adjustObjectCount(/*fIncrement*/ false);
        pInfo->adjustByteCount(-int64_t(o.sizeOf()));
        }
    }

void ClassBasedHeapAnalyzer::registerImmortal(const Object& o)
    {
    // Note: we could track imortals by class but the expectation is that the actual
    // number is quite low
    int64_t c = m_cImmortals.adjust(1, false);
    COH_LOG("Registering instance of " << Class::getTypeName(typeid(o))
            << " as an immortal; total immortal count is now " << c, 5);

    if (COH_LOGENABLED(9))
        {
        COH_LOG(Thread::formatStackTrace(Thread::getStackTrace()), 9);
        }
    }

// ----- nested class: ClassStats -------------------------------------------

// ----- constructors ---------------------------------------------------

ClassBasedHeapAnalyzer::ClassStats::ClassStats(int64_t cObjs, int64_t cBytes,
        int64_t cAllocs)
    : m_cInstanceCount(cObjs), m_cByteCount(cBytes), m_cAllocationCount(cAllocs)
    {
    }

// ----- ClassStats interface -------------------------------------------

int64_t ClassBasedHeapAnalyzer::ClassStats::getObjectCount() const
    {
    return m_cInstanceCount;
    }

int64_t ClassBasedHeapAnalyzer::ClassStats::getByteCount() const
    {
    return m_cByteCount;
    }

int64_t ClassBasedHeapAnalyzer::ClassStats::getAllocationCount() const
    {
    return m_cAllocationCount;
    }

// ----- Comparable interface -------------------------------------------

int32_t ClassBasedHeapAnalyzer::ClassStats::compareTo(Object::View v) const
    {
    ClassStats::View vThat  = cast<ClassStats::View>(v);
    int64_t          coThis = getObjectCount();
    int64_t          cbThis = getByteCount();
    int64_t          coThat = vThat->getObjectCount();
    int64_t          cbThat = vThat->getByteCount();

    return cbThis < cbThat
        ? -1
        : cbThis > cbThat
            ? 1
            : coThis < coThat
                ? -1
                : coThis > coThat
                    ? 1
                    : 0;
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> ClassBasedHeapAnalyzer::ClassStats::toString() const
    {
    return COH_TO_STRING("instances: " << getObjectCount() << ", bytes: " << getByteCount()
        << ", allocations: " << getAllocationCount());
    }

// ----- nested class: Snapshot ---------------------------------------------

// ----- constructors ---------------------------------------------------

ClassBasedHeapAnalyzer::Snapshot::Snapshot(TypedHandle<const Map> vMap,
        bool fShowAllocations)
        : f_vMapStats(self(), vMap), m_fShowAllocations(fShowAllocations)
    {
    }

TypedHandle<const Map> ClassBasedHeapAnalyzer::Snapshot::getStatsMap() const
    {
    return cast<Map::View>(f_vMapStats);
    }

int64_t ClassBasedHeapAnalyzer::Snapshot::getObjectCount() const
    {
    int64_t cTotal = 0;
    for (Iterator::Handle hIter = getStatsMap()->entrySet()->iterator();
            hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        ClassStats::View vStat  = cast<ClassStats::View>(vEntry->getValue());

        cTotal += vStat->getObjectCount();
        }

    return cTotal;
    }

HeapAnalyzer::Snapshot::View
ClassBasedHeapAnalyzer::Snapshot::delta(HeapAnalyzer::Snapshot::View vThat) const
    {
    return Snapshot::create(computeDelta<ClassInfo, ClassInfo>(getStatsMap(),
            cast<Snapshot::View>(vThat)->getStatsMap()), m_fShowAllocations);
    }


TypedHandle<const String> ClassBasedHeapAnalyzer::Snapshot::toString() const
    {
    bool    fShowAllocs = m_fShowAllocations;
    int64_t cObject     = 0;
    int64_t cByte       = 0;
    int64_t cAlloc      = 0;
    int64_t cClass      = 0;

    ObjectArray::Handle haEntries = getStatsMap()->entrySet()->toArray();

    Arrays::sort(haEntries, ClassBasedComparator::getInstance());

    String::View vs = String::create("Space\t\tCount");
    if (fShowAllocs)
        {
        vs = COH_TO_STRING(vs << "\t\tAllocs");
        }

    vs = COH_TO_STRING(vs << "\t\tClass" << std::endl);


    for (size32_t i = 0, c = haEntries->length; i < c; ++i)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(haEntries[i]);
        ClassStats::View vStats = cast<ClassStats::View>(vEntry->getValue());
        int64_t          co     = vStats->getObjectCount();
        int64_t          ca     = vStats->getAllocationCount();

        if (co != 0 || (fShowAllocs && ca != 0))
            {
            int64_t cb = vStats->getByteCount();

            cObject += co;
            cByte   += cb;
            cAlloc  += ca;
            cClass  += 1;

            vs = COH_TO_STRING(vs << (cb < 0 ? "-" : "")
                << StringHelper::toMemorySizeString((cb < 0 ? -cb : cb), false, true)
                << "\t\t" << co);
            if (fShowAllocs)
                {
                vs = COH_TO_STRING(vs << "\t\t" << ca);
                }
            vs = COH_TO_STRING(vs << "\t\t" << vEntry->getKey() << std::endl);
            }
        }

    vs = COH_TO_STRING(vs << "\nTotal: " << (cByte < 0 ? "-" : "")
        << StringHelper::toMemorySizeString((cByte < 0 ? -cByte : cByte), false, true)
        << ", " << cObject << " objects, ");
    if (fShowAllocs)
        {
        vs = COH_TO_STRING(vs << cAlloc << " allocations, ");
        }
    return COH_TO_STRING(vs << cClass << " classes");
    }

COH_CLOSE_NAMESPACE2
