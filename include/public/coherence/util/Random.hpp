/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RANDOM_HPP
#define COH_RANDOM_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic64.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic64;
using coherence::util::List;


/**
* An instance of this class is used to generate a stream of pseudorandom
* numbers.  This class is an implementation of D. H. Lehmer's linear
* congruential formula as described by Donald E. Knuth in:
* The Art of Computer Programming, Volume 2, Section 3.2.1.
*
* @author js  2008.03.27
*/
class COH_EXPORT Random
    : public class_spec<Random>
    {
    friend class factory<Random>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Random instance without supplying a seed.  The seed
        * will be initialized by the default algorithm provided in seed().
        */
        Random();

        /**
        * Create a new Random instance, specifying a seed to use.
        */
        Random(int64_t lSeed);


    // ----- Random interface -----------------------------------------------

    public:
        /**
        * Returns a pseudorandom, uniformly distributed int value between 0
        * (inclusive) and the specified value (exclusive), drawn from this
        * random number generator's sequence.
        *
        * @paran n  the upper bound (exclusive) for the returned value
        *
        * @return an int32_t value in the range [0, n)
        *
        * @throws IllegalArgumentException if n is <= 0
        */
        virtual int32_t nextInt32(int32_t n);

        /**
        * Returns the next pseudorandom, uniformly distributed boolean value
        * from this random number generator's sequence.
        *
        * @return the next bool value
        */
        virtual bool nextBoolean();

        /**
        * Returns the next pseudorandom, uniformly distributed int32_t value
        * from this random number generator's sequence.
        *
        * @return the next int32_t value in the range [Integer32::min_value,
        *         Integer32::max_value]
        */
        virtual int32_t nextInt32();

        /**
        * Returns the next pseudorandom, uniformly distributed int64_t value
        * from this random number generator's sequence.
        *
        * @return the next int64_t value in the range [Integer64::min_value,
        *         Integer64::max_value]
        */
        virtual int64_t nextInt64();

        /**
        * Returns the next pseudorandom, uniformly distributed float32_t
        * value from this random number generator's sequence.
        *
        * @return the next float32_t value in the range [0.0, 1.0]
        */
        virtual float32_t nextFloat32();

        /**
        * Returns the next pseudorandom, uniformly distributed float64_t
        * value from this random number generator's sequence.
        *
        * @return the next float64_t value in the range [0.0, 1.0]
        */
        virtual float64_t nextFloat64();

        /**
        * Provides a default seeding algorithm that is used to seed this
        * random number if no seed is provided.
        */
        virtual void seed();

        /**
        * Sets the seed of this random number generator using a single long
        * seed.
        */
        virtual void setSeed(int64_t lSeed);


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Randomize the order of the elements within the passed array.
        *
        * @param ha  an array of objects to randomize
        *
        * @return the array that was passed in, with its contents
        *         unchanged except for the order in which they appear
        *
        * @since Coherence 3.2
        */
        static ObjectArray::Handle randomize(ObjectArray::Handle ha);

        /**
        * Randomize the order of the elements within the passed list.
        *
        * @param ha  a list of objects to randomize
        *
        * @return the list that was passed in, with its contents
        *         unchanged except for the order in which they appear
        *
        * @since Coherence 3.4
        */
        static List::Handle randomize(List::Handle hl);

    protected:
        /**
        * Implementation of the random number algorithm which generates the
        * next pseudorandom number.
        *
        * Generates a 32 bit int value with it's corresponding bits being
        * individually generated pseudorandom 1's and 0's.  This method
        * should be overridden to provide a new random number algorithm.
        *
        * @param nBits  number of random bits to generate
        *
        * @return the next pseudorandom number with the specified number of
        *         randomly generated bits
        */
        virtual int32_t next(int32_t nBits);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return the static Random instance.
        *
        * @return the static Random instance
        */
        static Handle getInstance();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The internal state associated with this pseudorandom number
        * generator.
        */
        NativeAtomic64 m_atomicSeed;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_RANDOM_HPP

