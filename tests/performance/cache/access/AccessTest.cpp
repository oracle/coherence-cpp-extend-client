/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/ConfigurableCacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Hashtable.hpp"
#include "coherence/util/Random.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::CacheFactory;
using coherence::net::ConfigurableCacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::NamedCache;
using coherence::util::Collections;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Hashtable;
using coherence::util::Map;
using coherence::util::Random;
using coherence::util::SafeHashMap;
using coherence::util::Set;

/**
* Example of a complex data type.
*/
class ComplexValue
    : public class_spec<ComplexValue,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<ComplexValue>;

    protected:
        /**
        * Default constructor; needed for PortableObject.
        */
        ComplexValue()
            {}

        /**
        * Construct a ComplexValue with random values.
        *
        * @param nMax  the maximum allowed value for a field
        */
        ComplexValue(int32_t nMax)
            {
            Random::Handle hRand = Random::getInstance();

            for (size32_t i = 0; i < ARRAY_SIZE; ++i)
                {
                m_an[i] = hRand->nextInt32(nMax);
                }
            }

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            for (size32_t i = 0; i < ARRAY_SIZE; ++i)
                {
                m_an[i] = hIn->readInt32(i);
                }
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            for (size32_t i = 0; i < ARRAY_SIZE; ++i)
                {
                hOut->writeInt32(i, m_an[i]);
                }
            }

    private:
        static const size32_t ARRAY_SIZE = 25;
        int32_t               m_an[ARRAY_SIZE];
    };
COH_REGISTER_PORTABLE_CLASS(1234, ComplexValue);

class Worker
    : public class_spec<Worker,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<Worker>;

    protected:
        Worker(Map::Handle hCache,
               bool fPut,
               size32_t cPairBatch,
               int32_t  cbVal,
               size32_t iStart,
               size32_t iEnd,
               size32_t fCached)
            : m_hCache(self(), hCache),
              m_fPut(fPut),
              m_cPairBatch(cPairBatch),
              m_cbVal(cbVal),
              m_iStart(iStart),
              m_iEnd(iEnd),
              m_fCached(fCached),
              m_haBatch(self(), fCached
                      ? ObjectArray::create(((iEnd - iStart) / (cPairBatch == 0 ? 1: cPairBatch)) + 1)
                      : NULL),
              m_cOps(0),
              m_fRunning(false),
              m_fStop(false)
            {
            }

    // ----- Runnable interface ---------------------------------------------

    public:
        void run()
            {
            Map::Handle hCache     = m_hCache;
            bool        fPut       = m_fPut;
            size32_t    cPairBatch = m_cPairBatch;
            size32_t    cBatches   = (m_iEnd - m_iStart) / (cPairBatch == 0 ? 1: cPairBatch);
            ObjectArray::Handle haResult = ObjectArray::create(cPairBatch);

            m_fRunning = true;

            NamedCache::Handle hNamedCache;
            m_hCache = NULL;
            if (instanceof<NamedCache::Handle>(hCache))
                {
                hNamedCache = cast<NamedCache::Handle>(hCache);
                }

            // wait for start notification
            COH_SYNCHRONIZED (hCache)
                {
                hCache->wait();
                }

            try
                {
                while (!isStopRequested())
                    {
                    for (size32_t i = 0; i < cBatches && !isStopRequested(); ++i)
                        {
                        if (cPairBatch == 0)
                            {
                            if (fPut)
                                {
                                Map::Entry::View vEntry =
                                    cast<Map::Entry::View>(getEntryBatch(i));
                                hCache->put(vEntry->getKey(), vEntry->getValue());
                                }
                            else
                                {
                                hCache->get(getKeyBatch(i));
                                }
                            }
                        else
                            {
                            if (fPut)
                                {
                                hCache->putAll(cast<Map::View>(getEntryBatch(i)));
                                }
                            else
                                {
                                // ensure deserialization of values by storing
                                // the values in an array; this ensure that if
                                // if the cache using lazy deserialization ala
                                // ConverterMap, that we pay the deserialization
                                // costs
                                haResult = hNamedCache->getAll(cast<Set::View>(
                                        getKeyBatch(i)))->values()->toArray();
                                }
                            }

                        ++m_cOps;
                        }
                    }
                }
            catch (const std::exception& e)
                {
                m_fRunning = false;
                std::cerr << "Worker error: " << e.what() << std::endl;
                throw;
                }
            }


    // ----- Worker interface -----------------------------------------------

    public:
        Object::View getEntryBatch(size32_t nBatch)
            {
            bool fCached = m_fCached;

            if (fCached)
                {
                Object::View v = m_haBatch->raw[nBatch];

                if (v != NULL)
                    {
                    return v;
                    }
                }

            size32_t     cPairBatch = m_cPairBatch;
            Object::View vResult;

            if (cPairBatch == 0) // signifies non batching operations
                {
                int32_t   iKey = m_iStart + nBatch;
                Map::View vMap = Collections::singletonMap(
                        Integer32::create(iKey), createValue(iKey));
                vResult = vMap->entrySet()->iterator()->next();
                }
            else
                {
                Map::Handle hBatch = HashMap::create(cPairBatch);
                for (int i = m_iStart + (nBatch * cPairBatch), c = i + cPairBatch; i < c; ++i)
                    {
                    hBatch->put(Integer32::create(i), createValue(i));
                    }
                vResult = hBatch;
                }

            if (fCached)
                {
                m_haBatch->raw[nBatch] = vResult;
                }

            return vResult;
            }

        Object::View createValue(int32_t /*iKey*/)
            {
            int32_t cbVal = m_cbVal;
            return cbVal < 0
                ? (Object::View) ComplexValue::create(cbVal * -1)
                : (Object::View) Array<octet_t>::create(cbVal);
            }

        Object::View getKeyBatch(size32_t nBatch)
            {
            bool fCached = m_fCached;
            if (m_fCached)
                {
                Object::View v = m_haBatch->raw[nBatch];

                if (v != NULL)
                    {
                    return v;
                    }
                }

            size32_t     cPairBatch = m_cPairBatch;
            Object::View vResult;

            if (cPairBatch == 0) // signifies non batching operations
                {
                vResult = Integer32::create(m_iStart + nBatch);
                }
            else
                {
                Set::Handle hBatch = HashSet::create(cPairBatch);
                for (int i = m_iStart + (nBatch * cPairBatch), c = i + cPairBatch; i < c; ++i)
                    {
                    hBatch->add(Integer32::create(i));
                    }
                vResult = hBatch;
                }

            if (fCached)
                {
                m_haBatch->raw[nBatch] = vResult;
                }

            return vResult;
            }

        void stop()
            {
            m_fStop = true;
            }

        bool isStopRequested()
            {
            return m_fStop;
            }
    // ----- data members ---------------------------------------------------

    public:
        MemberHandle<Map>         m_hCache;
        const bool                m_fPut;
        const size32_t            m_cPairBatch;
        const int32_t             m_cbVal;
        const size32_t            m_iStart;
        const size32_t            m_iEnd;
        const bool                m_fCached;
        MemberHandle<ObjectArray> m_haBatch;

        int64_t                   m_cOps;
        bool                      m_fRunning;
        bool                      m_fStop;
    };

int main(int argc, char** argv)
    {
    if (argc <= 1)
        {
        std::cerr << "usage: <threads> <put(true) | get(false)> <batch size> "
              << "<entry byte size> <start key> <entries> <cached input> "
              << "<cache names ...>" << std::endl;
        return 1;
        }
    try
        {
        size32_t cThread    = argc > 1 ? (size32_t) Integer32::parse(argv[1]) : 1;
        bool     fPut       = argc > 2 ? Boolean::parse(argv[2]) : true;
        size32_t cPairBatch = argc > 3 ? (size32_t) Integer32::parse(argv[3]) : 1;
        int32_t  cbVal      = argc > 4 ? Integer32::parse(argv[4]) : 1;
        size32_t iStart     = argc > 5 ? (size32_t) Integer32::parse(argv[5]) : 0;
        size32_t cEntries   = argc > 6 ? (size32_t) Integer32::parse(argv[6]) : 1;
        bool     fCached    = argc > 7 ? Boolean::parse(argv[7]) : true;
        ObjectArray::Handle haName = ObjectArray::create(argc > 8 ? argc - 8 : 1);
        for (size32_t i = 8; i < (size32_t) argc; ++i)
            {
            haName[i - 8] = String::create(argv[i]);
            }
        if (argc == 8)
            {
            haName[0] = String::create("dist-access");
            }

        // very rough estimate of value size; this is the object form size
        // not the POF size.  Arguably this is the size that should be
        // used as it more closely reflects the user defined data versus
        // what we put on the network
        int32_t cbReal = cbVal < 0
            ? sizeof(ComplexValue) - sizeof(Object)
            : cbVal;

        std::cout
            << "threads: " << cThread << std::endl
            << " access: " << (fPut ? "put" : "get")
                << (cPairBatch == 0 ? "" : "All") << std::endl
            << "  batch: " << cPairBatch << std::endl
            << "  value: " << (cbVal < 0 ? "complex" : "byte[]") << std::endl
            << "  bytes: " << cbReal << std::endl
            << "  range: " << iStart << " .. " << (iStart + cEntries - 1) << std::endl
            << "  input: " << (fCached ? "cached" : "generated") << std::endl
            << "  cache: " << haName << std::endl << std::endl;

        ObjectArray::Handle haThreads = ObjectArray::create(cThread);
        ObjectArray::Handle haWorkers = ObjectArray::create(cThread);

        size32_t cPairPerWorker = cEntries / cThread;

        COH_ENSURE_RELATION(size32_t, cPairPerWorker, >=, cPairBatch);

        // find cache(s)
        ObjectArray::Handle haCache = ObjectArray::create(haName->length);
		Set::Handle         hCacheNames = HashSet::create();
        for (size32_t i = 0, c = haName->length; i < c; ++i)
            {
            String::View vsCache = cast<String::View>(haName[i]);
            if (vsCache->equals("map"))
                {
                haCache[i] = SafeHashMap::create();
                }
            else if (vsCache->equals("table"))
                {
                haCache[i] = Hashtable::create();
                }
            else if (hCacheNames->add(vsCache))
                {
                haCache[i] = CacheFactory::getCache(vsCache);
                }
            else
                {
                // same cache name listed multiple times, create new unshared accessor for it
                // if this is a remote cache it will have the same backing contents
                ConfigurableCacheFactory::Handle hFactory = DefaultConfigurableCacheFactory::create();
                hFactory->setConfig(CacheFactory::getConfigurableCacheFactory()->getConfig());
                haCache[i] = hFactory->ensureCache(vsCache);
                }
            }

        for (size32_t i = 0, c = haCache->length; i < c; ++i)
            {
            if (!fPut)
                {
                Map::Handle       hCache = cast<Map::Handle>(haCache[i]);
                int32_t           iEnd   = iStart + cEntries - 1;
                Integer32::Handle hEnd   = Integer32::create(iEnd);

                std::cout << "loading records to " << haName[i] << std::endl;
                Worker::Handle hWorker = Worker::create(hCache, /*fPut*/ true,
                        cPairBatch, cbVal, iStart, iEnd + 1, /*fCached*/ false);
                Thread::Handle hThread = Thread::create(hWorker);
                hThread->start();
                Thread::sleep(1000);

                // start worker
                COH_SYNCHRONIZED (hCache)
                    {
                    hCache->notifyAll();
                    }

                while (!hCache->containsKey(hEnd))
                    {
                    std::cout << (hCache->size() * 100) / (cEntries * c)
                        << "% full" << std::endl;
                    Thread::sleep(1000);
                    }

                hWorker->stop();
                hThread->join();
                }
            }

        std::cout << "starting test" << std::endl;

        // create all workers
        for (size32_t i = 0; i < cThread; ++i)
            {
            Map::Handle hCache = cast<Map::Handle>(haCache[i % haCache->length]);
            size32_t iStartW = iStart + (i * cPairPerWorker);
            size32_t iEnd    = (i == cThread - 1) ? iStart + cEntries : iStartW + cPairPerWorker;
            std::cout << "Thread " << i+1  << " range [" << iStartW << " ... " << iEnd << ") "
                      << " cache " << hCache << "@" << System::identityHashCode(hCache) << std::endl;
            Worker::Handle hWorker = Worker::create(hCache, fPut, cPairBatch, cbVal, iStartW, iEnd, fCached);
            haThreads[i] = Thread::create(hWorker);
            haWorkers[i] = hWorker;
            }

        if (cPairBatch == 0)
            {
            cPairBatch = 1;
            }

        // start all threads
        for (size32_t i = 0; i < cThread; ++i)
            {
            cast<Thread::Handle>(haThreads[i])->start();
            }

        // wait for all threads to be ready
        for (size32_t i = 0; i < cThread; ++i)
            {
            if (!cast<Worker::Handle>(haWorkers[i])->m_fRunning)
                {
                i = 0;
                std::cout << '.';
                std::cout.flush();
                Thread::sleep(10);
                }
            }

        // start all workers
        for (size32_t i = 0, c = haCache->length; i < c; ++i)
            {
            Object::View vCache = haCache[i];
            COH_SYNCHRONIZED (vCache)
                {
                vCache->notifyAll();
                }
            }

        Map::Handle hCache = cast<Map::Handle>(haCache[0]);
        haCache = NULL;

        HeapAnalyzer::View           vHeap = System::getHeapAnalyzer();
        HeapAnalyzer::Snapshot::View vPre  = NULL == vHeap ? NULL : vHeap->capture();

        // threads will never complete; sleep and periodically compute stats
        int64_t cMillisStart = System::currentTimeMillis();
        int64_t cOpsStart    = 0;

        while (true)
            {
            Thread::sleep(5000);

            int64_t cOpsEnd = 0;
            for (size32_t i = 0; i < cThread; ++i)
                {
                Worker::Handle hWorker = cast<Worker::Handle>(haWorkers[i]);
                COH_ENSURE(hWorker->m_fRunning);
                int64_t cWorkerOps = hWorker->m_cOps;
                std::cout << "Thread " << i + 1 << ": " << cWorkerOps << std::endl;
                cOpsEnd += cWorkerOps;
                }
            int64_t cMillisEnd = System::currentTimeMillis();

            int64_t   cOps       = cOpsEnd - cOpsStart;
            int64_t   cMillis    = cMillisEnd - cMillisStart;
            int64_t   cOpsPerSec = (cOps * 1000) / cMillis;
            int64_t   cEntPerSec = cOpsPerSec * cPairBatch;
            float64_t cMBPerSec  = (float64_t(cEntPerSec * cbReal)) / (1024 * 1024);
            float64_t cMsPerOp   = float64_t(cMillis) / float64_t(cOps / cThread);

            if (NULL != vHeap)
                {
                std::cout << vHeap->delta(vPre) << std::endl;
                vPre = NULL;
                vPre = vHeap->capture();
                }

            std::cout << "\nover " << (cMillis / 1000.0) << "s; performed "
                << cOps << " operations across " << cThread << " threads"
                << "\n\tthroughput "
                << cOpsPerSec << " ops/sec; "
                << cEntPerSec << " entries/sec; ~"
                << cMBPerSec << " MB/sec; "
                << "latency " << cMsPerOp << " ms/op; "
                << (hCache->size() * 100 / cEntries) << "% full"
                << std::endl;

            cMillisStart = cMillisEnd;
            cOpsStart    = cOpsEnd;
            }
        }
    catch (const std::exception& e)
        {
        std::cerr << e.what() << std::endl;
        return 1;
        }
    return 0;
    }
