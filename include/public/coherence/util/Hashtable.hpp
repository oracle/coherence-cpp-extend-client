/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HASHTABLE_HPP
#define COH_HASHTABLE_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/Set.hpp"


COH_OPEN_NAMESPACE2(coherence,util)

/**
* An implementation of coherence::util::Map that is thread-safe, but unlike
* SafeHashMap does not attempt to provide stable iteration in the presence
* of concurrent modifications.  Hashtable is an open-addressing based hash
* map implementation, relying on an array of pre-allocated entry structures.
* This approach significantly reduces the cost of insertion into the map as
* there is no Entry allocation required except as part of rehashing.  This
* optimization makes it an good candidate for short lived maps.
*
* Though thread-safe Hashtable is optimized for single-threaded access, and
* assumes that in most cases escape-analysis will elide the synchronization.
* If the Map will be accessed concurrently by many threads it is likely that
* SafeHashMap would be a better choice.  If the Map will largely live on a
* single thread, or be short lived then Hashtable is a good choice.
*
* Note: Hashtable's entryset iterator returns Map::Entry objects which are
* only valid until the iterator is advanced.  If the Entry needs to be
* retained longer then it should either be cloned or an shallow copy should
* be created.
*
* @author mf  2009.01.23
*
* @since Coherence 3.5
*/
class COH_EXPORT Hashtable
    : public cloneable_spec<Hashtable,
        extends<AbstractMap> >
    {
    friend class factory<Hashtable>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a thread-safe hash map using the specified settings.
        *
        * @param cEstimate        the anticipated number of elements that will
        *                         be stored
        * @param flLoadFactor     the acceptable load factor before resizing
        *                         occurs, 0 &lt; n, such that a load factor
        *                         of 1.0 causes resizing when the number of
        *                         entries exceeds the number of buckets
        * @param flGrowthRate     the rate of bucket growth when a resize
        *                         occurs, 0 &lt; n, such that a growth rate
        *                         of 1.0 will double the number of buckets:
        *                         bucketcount = bucketcount * (1 + growthrate)
        */
        Hashtable(size32_t cEstimate = 17, float32_t flLoadFactor = 0.75F,
                  float32_t flGrowthRate = 3.0F);

        /**
        * Constructs a new hash map with the same mappings as the specified Map.
        * The HashMap is created with default load factor (0.75) and an initial
        * capacity sufficient to hold the mappings in the specified Map.
        *
        * @param vMap  the map whose mappings are to be placed in this map
        *
        * @since 12.2.1
        */
        Hashtable(Map::View vMap);

        /**
        * Copy constructor.
        */
        Hashtable(const Hashtable& that);


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        using Map::get;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual void putAll(Map::View vMap);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The underlying storage.
        */
        FinalHandle<Map> f_hMapStorage;

        /**
        * The underlying storage's entry set.
        */
        FinalHandle<Set> f_hSetStorage;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_HASHTABLE_HPP
