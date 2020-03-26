/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/lang/ClassInfo.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/lang/AssociativeArray.hpp"
#include "private/coherence/native/NativeABI.hpp"

#include <map>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeABI;
using coherence::util::Map;
using coherence::util::SafeHashMap;


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return the Map representation of the AssociativeArray used by findByType.
    * This is a secondary copy and is only updated by findByType.
    */
    SafeHashMap::Handle getInfoMap()
        {
        static bool fInitialized  = false;
        static bool fInitializing = false;

        if (fInitialized == false)
            {
            if (fInitializing == true)
                {
                return NULL;
                }
            fInitializing = true;
            }

        // COH-16058 - allocate a SHM large enough to hold most/all classes
        //             to prevent recursion (due to the SHM growing) when registering classes
        static FinalHandle<SafeHashMap> hMap(System::common(),
                SafeHashMap::create(1031));

        if (!fInitialized)
            {
            // ensure that this object doesn't get destroyed during static
            // destroy
            hMap->_attach(/*fEscaped*/ true);

            fInitialized  = true;
            fInitializing = false;
            }

        return hMap;
        }
    COH_STATIC_INIT(getInfoMap());
    }


// ----- factory methods ----------------------------------------------------

ClassInfo::Handle ClassInfo::findByName(String::View vsClass)
    {
    Map::Handle hMap = getInfoMap();
    ClassInfo::Handle hInfo = cast<ClassInfo::Handle>(hMap->get(vsClass));
    if (NULL == hInfo)
        {
        COH_SYNCHRONIZED (hMap)
            {
            hInfo = cast<ClassInfo::Handle>(hMap->get(vsClass));
            if (NULL == hInfo)
                {
                hInfo = ClassInfo::create(vsClass);
                hMap->put(vsClass, hInfo);
                }
            }
        }
    return hInfo;
    }

ClassInfo* ClassInfo::findByType(const std::type_info& info,
        bool fEnsure)
    {
    static bool fInitialized  = false;
    static bool fInitializing = false;

    if (fInitialized == false)
        {
        if (fEnsure == false || fInitializing == true)
            {
            return NULL;
            }
        fInitializing = true;
        }

    typedef AssociativeArray<const std::type_info*, ClassInfo*> ClassInfoArray;
    static FinalHandle<ClassInfoArray>
        s_paaClass(System::common(), ClassInfoArray::create((ClassInfo*) NULL));
    static ClassInfoArray* const paaClass =
        get_pointer((ClassInfoArray::Handle) s_paaClass);
    static bool fAlive = paaClass->_attach(/*fEscaped*/ true) != NULL;
    const std::type_info* pInfo      = &info;
    ClassInfo*            pClassInfo = NULL;

    if (!fInitialized && fAlive)
        {
        fInitialized  = true;
        fInitializing = false;
        }

    // Note: &info (*std::type_info) is used as a key to ClassInfo map. While
    // this may seem incredibly unsafe, it actually is safe.  The C++ standard
    // states that any type_info& returned from typeid() will exist for the
    // life of the process. Additionally the only way to obtain a type_info
    // is from typeid(). The tricky part is tha there is no guarentee that
    // for a give type the same type_info& will be reruned to each call to
    // typeid(). So while it is a valid key, it is not a unique key
    // (VC++ does this). Obviously the number of copies of type_info for a
    // given type must be bounded by something or the C++ implmentation would
    // have a huge memory leak. The important part is that we can safely use
    // &type_info as a key, so long as on a miss we perform a secondary
    // (more expensive) check to see if the type is registered via another
    // key. This is done below, the result is very fast hash based lookups
    // based on &type_info.

    // Note: I'd love to include a link to where this is stated in the standard
    // if only I could find it.  What I have found is this discussion:
    // http://www.velocityreviews.com/forums/t286569-typeid-operator.html
    // Which says (fourth post) "the Standard in 5.2.8 says that the lvalue is
    // returned [from typeid] and that the lifetime of the object referred to
    // by the lvalue is entire program."
    // additionally since typeid() absolutely does return a reference this
    // basically makes sense, as any function which returns a refernce from
    // nowhere pretty much must be returning a reference to a global.

    // read block
        {
        SynchronizedMemberReadBlock::Guard readLock(*paaClass);
        pClassInfo = paaClass->get(pInfo);
        }

    if (NULL == pClassInfo && fEnsure)
        {
        // Register the key, this block should run once per class, or more
        // technically up to a few times per class (see note above)
        SafeHashMap::Handle hMapInfo = getInfoMap();
        if (NULL == hMapInfo)
            {
            // occurs during static destruction
            return NULL;
            }

        // assume we will write, create new objects outside of lock, so
        // that we hold the lock for as short a time as possible
        String::View      vKey  = NativeABI::demangle(info.name());
        ClassInfo::Handle hInfo = ClassInfo::create(vKey);

        // We have two maps (AssociativeArray<*type_info, ClassInfo>, and
        // SafeHashMap<String(class name), ClassInfo>, the later being the
        // master. The SHM ensures that there is only ever one ClassInfo
        // registered for a given class. The AA may contain multiple *type_info
        // elements for a given type, but all MUST reference the same ClassInfo
        // as the SHM. The AA is used for very fast, non-managed lookups.
        COH_SYNCHRONIZED (hMapInfo)
            {
            Map::Entry::Handle hEntry = hMapInfo->getEntry(vKey);
            if (NULL == hEntry)
                {
                // initial registration; this is the only code block allowed
                // to update hMapInfo
                if (NULL != hMapInfo->put(vKey, hInfo))
                    {
                    // can't happen unless locks are busted
                    COH_THROW (IllegalStateException::create(vKey));
                    }

                // update the associative array
                // write block (block AA readers)
                    {
                    SynchronizedMemberWriteBlock::Guard writeLock(*paaClass);
                    ClassInfo*& pValue = paaClass->at(pInfo);
                    if (NULL == pValue)
                        {
                        pValue = pClassInfo = get_pointer(hInfo);
                        }
                    else
                        {
                        // can't happen unless locks are busted
                        COH_THROW (IllegalStateException::create(vKey));
                        }
                    }
                }
            else
                {
                // already registered, see Note above, register the additional
                // key, but to the same value. This could actually overrite
                // the same key in a race condition, but that is fine

                // update the associative array
                // write block (block AA readers)
                    {
                    SynchronizedMemberWriteBlock::Guard writeLock(*paaClass);
                    pClassInfo = get_pointer(cast<ClassInfo::Handle>(
                            hEntry->getValue()));
                    paaClass->at(pInfo) = pClassInfo;
                    }
                }
            }
        }

    return pClassInfo;
    }
COH_STATIC_INIT(ClassInfo::findByType(typeid(Object), true));


// ----- constructors -------------------------------------------------------

ClassInfo::ClassInfo(String::View vsName)
        : f_vsName(self(), vsName), m_cInstances(0), m_cBytes(0), m_cAllocations(0),
          m_cbShallow(0)
    {
    }


// ----- ClassInfo interface ------------------------------------------------

String::View ClassInfo::getClassName() const
    {
    return f_vsName;
    }

Map::View ClassInfo::getClassInfoMap()
    {
    return getInfoMap();
    }

size32_t ClassInfo::getSize() const
    {
    return m_cbShallow;
    }

void ClassInfo::setSize(size32_t cb)
    {
    COH_ENSURE_EQUALITY(m_cbShallow, 0);
    m_cbShallow = cb;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ClassInfo::toString() const
    {
    return COH_TO_STRING(getClassName() << ": " << getObjectCount());
    }

COH_CLOSE_NAMESPACE2
