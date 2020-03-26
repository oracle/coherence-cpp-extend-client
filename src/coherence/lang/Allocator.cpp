/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "private/coherence/lang/Allocator.hpp"

#include "coherence/lang/IllegalStateException.hpp"
#include "coherence/lang/OutOfMemoryError.hpp"
#include "coherence/lang/Thread.hpp"

#include "private/coherence/native/NativeThreadLocal.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <climits>
#include <iostream>
#include <stdlib.h>

COH_OPEN_NAMESPACE2(coherence,lang)

using namespace coherence::native;


// ----- file local helpers -------------------------------------------------

/**
* Called when each thread exits, this function deletes the TLS memory pool.
*
* @param pPool
*/
extern "C" void coh_tls_pool_cleanup(void* pPool);

namespace
    {
    /**
    * Return the per-thread pool size, i.e. number of slots.
    */
    size_t getMemoryPoolSlotCount()
        {
        // Note: we can't use System::getProperty() here because that would
        // involve Object creation, and we'd endlessly recurse, we need to
        // return a single stable value for the life of the process
        // COH-12944 requires checking system property prefix for either coherence or tangosol
        static const char* achSlotsTmp1 = getenv("coherence.heap.slot.count");
        static const char* achSlotsTmp2 = achSlotsTmp1 == NULL
            ? getenv("CoherenceHeapSlotCount")
            : achSlotsTmp1;
        static const char* achSlotsTmp3 = achSlotsTmp2 == NULL
            ? getenv("tangosol.coherence.heap.slot.count")
            : achSlotsTmp2;
        static const char* achSlots     = achSlotsTmp3 == NULL
            ? getenv("TangosolCoherenceHeapSlotCount")
            : achSlotsTmp3;
        static const unsigned long lSlots = achSlots == NULL ? 8192 : strtoul(achSlots, NULL, 10);
        static const size_t cSlots = (lSlots < ULONG_MAX) ? (size_t) lSlots : 8192;
        return cSlots;
        }
    COH_STATIC_INIT(getMemoryPoolSlotCount());

    /**
    * Return the per-slot memory size.
    *
    * Ideally this will be a multiple of the processors cache-line size. This
    * value is not easily identified at runtime, but for most processors will
    * be a multiple of 64.
    *
    * Target the default slot count * slot size to be approx 1MB, resulting
    * in a 1MB memory pool per thread.
    */
    size_t getMemoryPoolSlotSize()
        {
        // Note: we can't use System::getProperty() here because that would
        // involve Object creation, and we'd endlessly recurse, we need to
        // return a single stable value for the life of the process
        // COH-12944 requires checking system property prefix for either coherence or tangosol
        static const char* achSlotTmp1 = getenv("coherence.heap.slot.size");
        static const char* achSlotTmp2 = achSlotTmp1 == NULL
            ? getenv("CoherenceHeapSlotSize")
            : achSlotTmp1;
        static const char* achSlotTmp3 = achSlotTmp2 == NULL
            ? getenv("tangosol.coherence.heap.slot.size")
            : achSlotTmp2;
        static const char* achSlot     = achSlotTmp3 == NULL
            ? getenv("TangosolCoherenceHeapSlotSize")
            : achSlotTmp3;
        static const unsigned long lbSlot = achSlot == NULL ? 128 : strtoul(achSlot, NULL, 10);
        static const size_t cbSlot = (lbSlot < ULONG_MAX) ? (size_t) lbSlot : 128;
        return cbSlot;
        }
    COH_STATIC_INIT(getMemoryPoolSlotSize());

    /**
    * Return the slot refill rate.
    *
    * This value specifies how many slot misses must occur before an allocation
    * is increased to slot size in order to help refill the pool.
    */
    size_t getMemoryPoolSlotRefill()
        {
        // Note: we can't use System::getProperty() here because that would
        // involve Object creation, and we'd endlessly recurse, we need to
        // return a single stable value for the life of the process
        // COH-12944 requires checking system property prefix for either coherence or tangosol
        static const char* achSlotTmp1 = getenv("coherence.heap.slot.refill");
        static const char* achSlotTmp2 = achSlotTmp1 == NULL
            ? getenv("CoherenceHeapSlotRefill")
            : achSlotTmp1;
        static const char* achSlotTmp3 = achSlotTmp2 == NULL
            ? getenv("tangosol.coherence.heap.slot.refill")
            : achSlotTmp2;
        static const char* achSlot     = achSlotTmp3 == NULL
            ? getenv("TangosolCoherenceHeapSlotRefill")
            : achSlotTmp3;
        static const size_t cRefill = achSlot == NULL
            ? 10000 : atoi(achSlot);
        return cRefill;
        }
    COH_STATIC_INIT(getMemoryPoolSlotRefill());

    /**
    * Return the amount of padding which should be added to the front and
    * back of each allocated object.
    */
    size_t getMemoryPadSize(bool fPre)
        {
        static size_t cbPadPre;
        static size_t cbPadPost;

        static bool fInit = true;
        if (fInit)
            {
            fInit = false; // compute only once
            // Note: we can't use System::getProperty() here because that would
            // involve Object creation, and we'd endlessly recurse, we need to
            // return a single stable value for the life of the process
            // COH-12944 requires checking system property prefix for either coherence or tangosol
            const char* achPadTmp1 = getenv("coherence.heap.padding");
            const char* achPadTmp2 = achPadTmp1 == NULL
                ? getenv("CoherenceHeapPadding")
                : achPadTmp1;
            const char* achPadTmp3 = achPadTmp2 == NULL
                ? getenv("tangosol.coherence.heap.padding")
                : achPadTmp2;
            const char* achPad     = achPadTmp3 == NULL
                ? getenv("TangosolCoherenceHeapPadding")
                : achPadTmp3;
            const unsigned long lbPad = achPad == NULL
                ? (strcmp(COH_SYMB_TO_STRING(COH_BUILD_TYPE), "Release") == 0
                    ? 0 : sizeof(size_t) * 3)
                : strtoul(achPad, NULL, 10);
            const size_t cbPad = (lbPad == ULONG_MAX)
                ? (strcmp(COH_SYMB_TO_STRING(COH_BUILD_TYPE), "Release") == 0
                    ? 0 : sizeof(size_t) * 3)
                : (size_t) lbPad;
            const size_t cbWord = sizeof(size_t);

            // choose pre and post pad sizes, pre size MUST be a multiple of
            // the word size to avoid any alignment issues, so we will round
            // the pre-pad up to the next word boundary as necessary

            // Solaris SPARC has alignment issues with 4 byte word
            // alignment.  Set cbPadPre to be 8 byte aligned
            if (cbPad <= cbWord) // small or no pad
                {
                // pad will be 0, 1, or 2 words
                // format: [size] <data>
                cbPadPre  = 0;
                cbPadPost = 0; // no post pattern

                if (cbPad > 0)
                    {
                    cbPadPre += cbWord; // aligned (may be 4)
                    }
                if (getMemoryPoolSlotCount() > 0)
                    {
                    cbPadPre += cbWord; // aligned (may be 4)
                    }

                if (cbPadPre < 8 && cbPadPre > 0)
                  {
                  cbPadPre = 8; // 8 byte aligned
                  }
                }
            else if (cbPad < cbWord * 4) // medium pad
                {
                // format: <size> <pattern> <hash 1B> <data>
                cbPadPre  = 2 * cbWord; // 8 byte aligned
                cbPadPost = cbPad < cbPadPre ? 0 : cbPad - cbPadPre;
                }
            else // big pad (> 4 words) (pre and post pattern)
                {
                // format: <size> <pre-pattern> <hash 1B> <data> <post-pattern>
                // pre must be at least two words
                cbPadPre  = cbWord * (cbPad / (2 * cbWord)); // aligned (may be 4)
                if (cbPadPre % 8 == 4)
                  {
                  cbPadPre += 4;  // 8 byte aligned
                  }
                cbPadPost = cbPad - cbPadPre;
                }

            if (cbPad > 0 && achPad != NULL)
                {
                // user specified a pad size, log it
                // can't use logger at this stage, write directly
                std::cerr << COH_SYMB_TO_STRING(COH_PRODUCT) << ' ' <<
                    COH_SYMB_TO_STRING(COH_VERSION) <<
                    ": enabling " << cbPadPre << "/" << cbPadPost <<
                    "B pre/post Object padding" << std::endl;
                }
            }

        return fPre ? cbPadPre : cbPadPost;
        }
    COH_STATIC_INIT(getMemoryPadSize(true));

    /**
    * Return whether to output allocation debugging.
    */
    bool isHeapLogging()
        {
        static bool fHeapLogging;

        static bool fInit = true;
        if (fInit)
            {
            fInit = false; // compute only once
            // Note: we can't use System::getProperty() here because that would
            // involve Object creation, and we'd endlessly recurse, we need to
            // return a single stable value for the life of the process
            // COH-12944 requires checking system property prefix for either coherence or tangosol
            const char* achHeapLoggingTmp1 = getenv("coherence.heap.logging");
            const char* achHeapLoggingTmp2 = achHeapLoggingTmp1 == NULL
                ? getenv("CoherenceHeapLogging")
                : achHeapLoggingTmp1;
            const char* achHeapLoggingTmp3 = achHeapLoggingTmp2 == NULL
                ? getenv("tangosol.coherence.heap.logging")
                : achHeapLoggingTmp2;
            const char* achHeapLogging     = achHeapLoggingTmp3 == NULL
                ? getenv("TangosolCoherenceHeapLogging")
                : achHeapLoggingTmp3;
            fHeapLogging = achHeapLogging == NULL
                    ? false
                    : ((strcmp(achHeapLogging, "true") == 0)
                            ? true : false);
            if (fHeapLogging)
                {
                // user specified heap logging, log it
                // can't use logger at this stage, write directly
                std::cerr << COH_SYMB_TO_STRING(COH_PRODUCT) << ' ' <<
                    COH_SYMB_TO_STRING(COH_VERSION) <<
                    ": enabling thread-local allocator heap logging" << std::endl;
                }
            }

        return fHeapLogging;
        }
    COH_STATIC_INIT(isHeapLogging());

    /**
    * Thread local memory pool
    */
    class Pool
        {
        public:
            Pool(size_t cSlots, size_t cbSlot, size_t cRefill, bool fHeapLogging)
                : m_cSlots(cSlots), m_cbSlot(cbSlot), m_cRefill(cRefill),
                  m_apSlots(NULL), m_of(cSlots), m_cMisses(0), m_cHits(0),
                  m_fHeapLogging(fHeapLogging)
                {
                if (cSlots <= ULONG_MAX /sizeof(void*))
                    {
                    void** apSlots = (void**) ::calloc(cSlots, sizeof(void*));
                    OutOfMemoryError::ensure(apSlots, cSlots * sizeof(void*));
                    m_apSlots = apSlots;
                    }
                }

            ~Pool()
                {
                void** apSlots = m_apSlots;
                if (apSlots != NULL)
                    {
                    for (size_t i = 0, c = m_cSlots; i < c; ++i)
                        {
                        ::free(apSlots[i]);
                        }
                    ::free(apSlots);
                    }
                }

            /**
            * Allocate a block of memory from the pool.
            *
            * @return a block of memory, or NULL if none was available
            */
            void* allocate()
                {
                size_t of      = m_of;
                void** apSlots = m_apSlots;

                if (of < m_cSlots)
                    {
                    void* ab    = apSlots[of];
                    apSlots[of] = NULL;
                    m_of = of + 1;
                    ++m_cHits;
                    if (m_fHeapLogging && (m_cHits % 1000 == 0))
                        {
                        std::cerr << "(thread=" << Thread::currentThread()->getName()
                            << "): Allocator hit: pool=" << this << ", size="
                            << m_cbSlot << ", slots="
                            << m_cSlots << ", hit rate="
                            << (m_cHits / ((float32_t) m_cMisses + m_cHits))
                            << std::endl;
                        }
                    return ab;
                    }

                // pool was empty
                size_t cRefill = m_cRefill;
                size_t cMisses = ++m_cMisses;
                if (cRefill != 0 && (cMisses % cRefill == 0))
                    {
                    if (m_fHeapLogging)
                        {
                        std::cerr << "(thread=" << Thread::currentThread()->getName()
                            << "): Allocator alloc: pool=" << this << ", size="
                            << m_cbSlot << ", slots="
                            << m_cSlots << ", hit rate="
                            << (m_cHits / ((float32_t) m_cMisses + m_cHits))
                            << std::endl;
                        }
                    // slowly replenish the pools
                    return ::malloc(m_cbSlot);
                    }
                return NULL;
                }

            /**
            * Release a block of memory back into the pool
            *
            * @param ab  a block to release
            *
            * @return true if the pool accepted the block, false otherwise
            */
            bool release(void* ab)
                {
                if (ab != NULL)
                    {
                    size_t of      = m_of;
                    void** apSlots = m_apSlots;

                    if (of > 0)
                        {
                        --of;
                        apSlots[of] = ab;
                        m_of = of;

                        return true;
                        }

                    // the pool is full
                    }
                return false;
                }

        protected:
            /**
            * The number of slots.
            */
            size_t m_cSlots;

            /**
            * The slot size.
            */
            size_t m_cbSlot;

            /**
            * The number of misses which must occur before a refill allocation
            * is made.
            */
            size_t m_cRefill;

            /**
            * The array of slots.
            */
            void** m_apSlots;

            /**
            * The slot offset.
            */
            size_t  m_of;

            /**
            * The number of allocation misses.
            */
            size_t m_cMisses;

            /**
            * The number of allocation hits.
            */
            size_t m_cHits;

            /**
            * Whether or not to log the heap allocations.
            */
            bool m_fHeapLogging;
        };

    /**
    * Return the system's singleton always empty pool.
    */
    Pool& getSystemPool()
        {
        // the system pool is always empty, so it will redirect to malloc/free
        static Pool poolSystem(0, 0, 0, isHeapLogging());
        return poolSystem;
        }
    COH_STATIC_INIT(getSystemPool());

    Pool& getTLSPool(); // forward decl

    /**
    * Helper class for initializing and cleaning up the TLS used to store the
    * current thread.
    */
    class TLSManager
        {
        public:
            TLSManager()
                {
                s_pTLS = NativeThreadLocal::create(&coh_tls_pool_cleanup);
                }

            ~TLSManager()
                {
                // Free the pool for this thread
                Pool* pPoolSystem = &(getSystemPool());
                Pool* pPoolThread = &(getTLSPool());

                if (pPoolSystem != pPoolThread)
                    {
                    delete pPoolThread;
                    }

                // free up the TLS (a very limited resource)
                delete s_pTLS;
                s_pTLS = NULL;
                }

            /**
            * Return the NativeThreadLocal used to find the ThreadLocal Pool
            * associated with the calling thread.
            *
            * @return the NativeThreadLocal
            */
            static NativeThreadLocal* getTLS()
                {
                static TLSManager singleton;
                return s_pTLS;
                }

        private:
            static NativeThreadLocal* s_pTLS;
        };
    NativeThreadLocal* TLSManager::s_pTLS = NULL;
    COH_STATIC_INIT(TLSManager::getTLS());

    /**
    * Return the memory pool for the current thread.
    */
    Pool& getTLSPool()
        {
        NativeThreadLocal* pTLS  = TLSManager::getTLS();
        Pool*              pPool = pTLS == NULL
                ? &(getSystemPool())
                : (Pool*) pTLS->get();

        if (NULL == pPool)
            {
            pPool = new Pool(getMemoryPoolSlotCount(),
                    getMemoryPoolSlotSize(), getMemoryPoolSlotRefill(),
                    isHeapLogging());
            pTLS->set(pPool);
            }
        return *pPool;
        }
    }

extern "C" void coh_tls_pool_cleanup(void* pPool)
    {
    Pool* pPoolSystem = &getSystemPool();

    if (pPool != pPoolSystem)
        {
        delete (Pool*) pPool;

        // in case there are any more allocs or frees before the thread completely
        // dies we need to redirect all remaining allocations to the system pool
        NativeThreadLocal* pTLS  = TLSManager::getTLS();
        if (pTLS != NULL)
            {
            pTLS->set(pPoolSystem);
            }
        }
    }

void* Allocator::allocate(size_t cb)
    {
    static const size_t cbSlot       = getMemoryPoolSlotSize();
    static const size_t cbPadPre     = getMemoryPadSize(true);
    static const size_t cbPadPost    = getMemoryPadSize(false);
    static const size_t cbPadTotal   = cbPadPre + cbPadPost;
    static const bool   fEncodeSize  = cbPadPre > 0;
    static const bool   fValidatePre = cbPadPre > sizeof(size_t)
        && cbPadPre > 8;

    if (cb > ULONG_MAX - cbPadTotal)
        {
        return NULL;
        }
    size_t cbAlloc = cb + cbPadTotal;

    void* ab = NULL;
    if (cbAlloc <= cbSlot)
        {
        // eligible for pooled allocation
        // TODO: add support for N pools
        ab = getTLSPool().allocate();

        if (ab)
            {
            cbAlloc = cbSlot;
            cb      = cbSlot - cbPadTotal;
            }
        }

    if (NULL == ab)
        {
        // pool miss, allocate from system
        ab = ::malloc(cbAlloc);
        OutOfMemoryError::ensure(ab, cbAlloc);
        }

    // compute return pointer
    void* abRet = ((unsigned char*) ab) + cbPadPre;

    if (fEncodeSize)
        {
        // encode size
        // format: <size> [pre-pattern] [hash] <data> [post-pattern]
        ::memcpy(ab, &cb, sizeof(size_t));

        unsigned char* abPad = (unsigned char*) ab;
        if (fValidatePre)
            {
            // write a detectable pattern over the pad bytes
            // format: <size> <pre-pattern> <hash> <data> [post-pattern]
            unsigned char chHash = 0;
            size_t        ofPad  = 0;

            // hash the encoded size
            ofPad = sizeof(size_t);
            for (size_t i = 0; i < ofPad; ++i)
                {
                chHash += abPad[i];
                }

            // write pre-pattern
            for (size_t i = ofPad, c = cbPadPre - 1; i < c; ++i)
                {
                chHash += (abPad[i] = (unsigned char) ((i + 1) % 255));
                }

            // write hash
            abPad[cbPadPre - 1] = chHash;
            }

        // write pattern over entire post section
        if (cbPadPost > 0)
            {
            abPad += (cb + cbPadPre);
            for (size_t i = 0; i < cbPadPost; ++i)
                {
                abPad[i] = (unsigned char) ((i + 1) % 255);
                }
            }
        }

    // as any decent language would do, we clear the memory before returning
    // Though not documented, this should be considered a contract, and cannot
    // be removed, or made conditional.
    ::memset(abRet, 0, cb);

    return abRet;
    }

void Allocator::release(void* ab)
    {
    static const size_t cbSlot       = getMemoryPoolSlotSize();
    static const size_t cbPadPre     = getMemoryPadSize(true);
    static const size_t cbPadPost    = getMemoryPadSize(false);
    static const size_t cbPadTotal   = cbPadPre + cbPadPost;
    static const bool   fEncodedSize = cbPadPre > 0;
    static const bool   fValidatePre = cbPadPre > sizeof(size_t)
        && cbPadPre > 8;

    // actual allocated pointer
    void* abFree;

    if (fEncodedSize)
        {
        abFree = ((unsigned char*) ab) - cbPadPre;

        if (fValidatePre)
            {
            // read and validate pre-pattern hash
            unsigned char* abPad = (unsigned char*) abFree;
            unsigned char chHash = 0;

            // calculate hash of pre-pattern
            for (size_t i = 0, c = cbPadPre - 1; i < c; ++i)
                {
                chHash += abPad[i];
                }

            // read/validate hash
            if (chHash != abPad[cbPadPre - 1])
                {
                COH_THROW_STREAM (IllegalStateException,
                    "memory corruption detected in " << cbPadPre << "B " <<
                    "pre-padding hash of "
                    << "memory allocated at " << std::hex << abFree);
                }
            }

        // read size
        size_t cb;
        ::memcpy(&cb, abFree, sizeof(size_t));
        size_t cbFree = cb + cbPadTotal;

        if (cbPadPost > 0)
            {
            // validate post pattern
            unsigned char* abPad = ((unsigned char*) abFree) + cbPadPre + cb;
            for (size_t i = 0; i < cbPadPost; ++i)
                {
                if (abPad[i] != (i + 1) % 255)
                    {
                    COH_THROW_STREAM (IllegalStateException,
                        "memory corruption detected in " << cbPadPost << "B " <<
                        "post-padding at offset " << i <<
                        " of memory allocated at " << std::hex << abFree);
                    }
                }

            // zero out the memory before freeing, this encourages segfault if
            // there are any dangling references, note unlike clearing during
            // alloc, this clear is done conditionally as it is meant to detect
            // dangling pointers, which is similar to detecting memory
            // corruption, so we only do that when those checks are enabled
            ::memset(abFree, 0, cbFree);
            }

        if (cbFree == cbSlot)
            {
            // eligible for release back into pool
            if (getTLSPool().release(abFree))
                {
                // the pool accepted the block
                return;
                }
            }
        }
    else
      {
      abFree = ab;
      }

    // release back to system
    ::free(abFree);
    }

COH_CLOSE_NAMESPACE2
