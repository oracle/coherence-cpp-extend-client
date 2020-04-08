/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HASH_HELPER_HPP
#define COH_HASH_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE2(coherence, util)

using coherence::util::Collection;

/**
* This abstract class contains helper functions for calculating hash code
* values for any group of C++ intrinsics.
*
* @author hr 2011.08.30
*
* @since Coherence 12.1.2
*/
class COH_EXPORT HashHelper
    : public abstract_spec<HashHelper,
        extends<Object> >
    {
    // ----- static functions -----------------------------------------------

    public:
        /**
        * Calculate a running hash using the boolean value.
        *
        * @param fValue  the boolean value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(bool fValue, size32_t nHash);

        /**
        * Calculate a running hash using the octet_t value.
        *
        * @param bValue  the octet_t value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(octet_t bValue, size32_t nHash);

        /**
        * Calculate a running hash using the wchar16_t value.
        *
        * @param chValue  the wchar16_t value for use in the hash
        * @param nHash    the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(wchar16_t chValue, size32_t nHash);

        /**
        * Calculate a running hash using the float64_t value.
        *
        * @param dflValue  the float64_t value for use in the hash
        * @param nHash     the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(float64_t dflValue, size32_t nHash);

        /**
        * Calculate a running hash using the float32_t value.
        *
        * @param flValue  the float32_t value for use in the hash
        * @param nHash    the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(float32_t flValue, size32_t nHash);

        /**
        * Calculate a running hash using the int32_t value.
        *
        * @param nValue  the int32_t value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(int32_t nValue, size32_t nHash);

        /**
        * Calculate a running hash using the int64_t value.
        *
        * @param nValue  the int64_t value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(int64_t lValue, size32_t nHash);

        /**
        * Calculate a running hash using the size32_t value.
        *
        * @param nValue  the size32_t value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(size32_t nValue, size32_t nHash);

        /**
        * Calculate a running hash using the size64_t value.
        *
        * @param lValue  the size64_t value for use in the hash
        * @param nHash   the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(size64_t lValue, size32_t nHash);

        /**
        * Calculate a running hash using the int16_t value.
        *
        * @param shValue  the int16_t value for use in the hash
        * @param nHash    the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(int16_t shValue, size32_t nHash);

        /**
        * Calculate a running hash using the Object value.
        *
        * @param vValue  the Object value for use in the hash
        * @param nHash    the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(Object::View vValue, size32_t nHash);

        /**
        * Calculate a running hash using the array delegating to the
        * runtime type for each element.
        *
        * @param ha     the array for use in the hash
        * @param nHash  the running hash value
        *
        * @return the resulting running hash value
        */
        template<class T> static size32_t
            hash(const TypedHandle<Array<T> >& ha, size32_t nHash)
            {
            nHash = swizzle(nHash);
            if (NULL == ha)
                {
                return nHash;
                }

            for (size32_t i = 0; i < ha->length; ++i)
                {
                nHash = hash((*ha)[i], nHash);
                }
            return nHash;
            }

        /**
        * Calculate a running hash using the ObjectArray value.
        *
        * @param avValue  the ObjectArray value for use in the hash
        * @param nHash    the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(ObjectArray::View avValue, size32_t nHash);

        /**
        * Calculate a running hash using the Collection value.
        *
        * @param vCol  the Collection value for use in the hash
        * @param nHash the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t hash(Collection::View vCol, size32_t nHash);

    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Shift the running hash value to try and help with
        * generating unique values given the same input, but
        * in a different order.
        *
        * @param nHash  the running hash value
        *
        * @return the resulting running hash value
        */
        static size32_t swizzle(size32_t nHash);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_HASH_HELPER_HPP
