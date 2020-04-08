/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Random.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/AtomicCounter.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Used in seed calculation
    */
    static const int64_t MULTIPLIER = COH_INT64(0x5U, 0xDEECE66DU);

    /**
    * Used in seed calculation
    */
    static const int64_t ADDEND = 0xBL;

    /**
    * Used in seed calculation
    */
    static const int64_t MASK = ((int64_t) 0x01 << 48) - 1;

    /**
    * Calculate the next seed.
    *
    * @return the next seed value
    */
    int64_t nextSeed()
        {
        static FinalHandle<AtomicCounter> hCounter(System::common(),
                AtomicCounter::create(COH_INT64(0x1ED8B5U,0x5FAC9DECU)));

        return hCounter->increment() + System::currentTimeMillis();
        }
    COH_STATIC_INIT(nextSeed());

    } // end of anonymous namespace


// ----- constants ----------------------------------------------------------

Random::Handle Random::getInstance()
    {
    static FinalHandle<Random> hRandom
        (System::common(), create());
    return hRandom;
    }
COH_STATIC_INIT(Random::getInstance());


// ----- constructors -------------------------------------------------------

Random::Random()
    : m_atomicSeed(0)
    {
    seed();
    }

Random::Random(int64_t lSeed)
    : m_atomicSeed(0)
    {
    setSeed(lSeed);
    }


// ----- Random interface ---------------------------------------------------

int32_t Random::nextInt32(int32_t n)
    {
    COH_ENSURE_PARAM_RELATION(n, >, 0);

    if ((n & -n) == n) // check if n is a power of 2
        {
        return (int32_t)((n * (int64_t) next(31)) >> 31);
        }

    int32_t nBits, nVal;
    do
        {
        nBits = next(31);
        nVal = nBits % n;
        }
    while(nBits - nVal + (n-1) < 0);

    return nVal;
    }

bool Random::nextBoolean()
    {
    return next(1) != 0;
    }

int32_t Random::nextInt32()
    {
    return next(32);
    }

int64_t Random::nextInt64()
    {
    return (int64_t) COH_INT64(next(32), next(32));
    }

float32_t Random::nextFloat32()
    {
    int32_t n = next(24);
    return n / ((float32_t)(1 << 24));
    }

float64_t Random::nextFloat64()
    {
    int64_t l = ((int64_t)(next(26)) << 27) + next(27);
    return l / (float64_t) ((int64_t) 1 << 53);
    }

void Random::setSeed(int64_t lSeed)
    {
    lSeed = (lSeed ^ MULTIPLIER) & MASK;
    m_atomicSeed.set(lSeed);
    }

void Random::seed()
    {
    setSeed(nextSeed());
    // spin the seed
    next(1);
    }

ObjectArray::Handle Random::randomize(ObjectArray::Handle ha)
    {
    size32_t c = 0;
    if (NULL == ha || (c = ha->length) <= 1)
        {
        return ha;
        }

    Random::Handle hRnd = getInstance();
    for (size32_t i1 = 0; i1 < c; ++i1)
        {
        // REVIEW: this could technically be improved by using a Random
        // method that corresponds with size32_t - this however requires
        // platform specialization
        size32_t i2 = (size32_t) hRnd->nextInt32((int32_t) c);

        // swap i1, i2
        Object::Holder o = ha[i2];
        ha[i2] = ha[i1];
        ha[i1] = o;
        }

    return ha;
    }

List::Handle Random::randomize(List::Handle hl)
    {
    ObjectArray::Handle haadr       = hl->toArray();
    ArrayList::Handle   hResultList = ArrayList::create();

    haadr = Random::randomize(haadr);

    for(size32_t i = 0, c = haadr->length; i < c; ++i)
        {
        hResultList->add(haadr[i]);
        }

    return hResultList;
    }

int32_t Random::next(int32_t nBits)
    {
    int64_t lOldSeed, lNextSeed;
    NativeAtomic64 *pAtomicSeed = &m_atomicSeed;
    do
        {
        lOldSeed  = pAtomicSeed->get();
        lNextSeed = (lOldSeed * MULTIPLIER + ADDEND) & MASK;
        }
    while (pAtomicSeed->update(lOldSeed, lNextSeed) != lOldSeed);

    return (int32_t) (uint64_t(lNextSeed) >> (48 - nBits));
    }

COH_CLOSE_NAMESPACE2

