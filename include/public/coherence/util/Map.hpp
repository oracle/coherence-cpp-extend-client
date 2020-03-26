/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_HPP
#define COH_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Set.hpp"

#include "coherence/io/pof/PofIntrinsic.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* An interface for associating key value pairs.
*
* @see Collection
* @see Set
*/
class COH_EXPORT Map
    : public interface_spec<Map,
      implements<coherence::io::pof::PofIntrinsic> >
    {
    // ----- nested interface: Entry ----------------------------------------

    public:

        /**
        * A map entry (key-value pair). The <tt>Map::entrySet</tt> method
        * returns a collection-view of the map, whose elements are of this
        * class. The <i>only</i> way to obtain a reference to a map entry is
        * from the iterator of this collection-view. These
        * <tt>Map::Entry</tt> objects are valid <i>only</i> for the duration
        * of the iteration; more formally, the behavior of a map entry is
        * undefined if the backing map has been modified after the entry was
        * returned by the iterator.
        *
        * @see Map#entrySet()
        */
        class COH_EXPORT Entry
            : public interface_spec<Entry>
            {
            // ----- Map::Entry interface -----------------------------------

            public:
                /**
                * Return the key corresponding to this entry.
                *
                * @return the key corresponding to this entry.
                */
                virtual Object::View getKey() const = 0;

                /**
                * Return the value corresponding to this entry.
                *
                * @return the value corresponding to this entry.
                */
                virtual Object::Holder getValue() const = 0;

                /**
                * Return the value corresponding to this entry.
                *
                * @return the value corresponding to this entry.
                */
                virtual Object::Holder getValue() = 0;

                /**
                * Update the value corresponding to this entry.
                *
                * @param ohValue  the new value for the entry
                *
                * @return the prior value
                */
                virtual Object::Holder setValue(Object::Holder ohValue) = 0;
            };


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * Return the number of key-value mappings in this map.
        *
        * @return the number of key-value mappings in this map.
        */
        virtual size32_t size() const = 0;

        /**
        * Return <tt>true</tt> if this map contains no key-value mappings.
        *
        * @return <tt>true</tt> if this map contains no key-value mappings.
        */
        virtual bool isEmpty() const = 0;

        /**
        * Return <tt>true</tt> if this map contains a mapping for the
        * specified key.
        *
        * @param vKey key whose presence in this map is to be tested.
        *
        * @return <tt>true</tt> if this map contains a mapping for the
        *         specified key.
        */
        virtual bool containsKey(Object::View vKey) const = 0;

        /**
        * Return <tt>true</tt> if this map maps one or more keys to the
        * specified value.  This operation will probably require time linear
        * in the map size for most implementations of the <tt>Map</tt>
        * interface.
        *
        * @param vValue value whose presence in this map is to be tested.
        *
        * @return <tt>true</tt> if this map maps one or more keys to the
        *         specified value.
        */
        virtual bool containsValue(Object::View vValue) const = 0;

        /**
        * Return the value to which this map maps the specified key. Return
        * <tt>NULL</tt> if the map contains no mapping for this key. A return
        * value of <tt>NULL</tt> does not <i>necessarily</i> indicate that
        * the map contains no mapping for the key; it's also possible that
        * the map explicitly maps the key to <tt>NULL</tt>.
        * The <tt>containsKey</tt> operation may be used to distinguish these
        * two cases.
        *
        * @param vKey key whose associated value is to be returned.
        *
        * @return the value to which this map maps the specified key, or
        *         <tt>NULL</tt> if the map contains no mapping for this key.
        *
        * @see #containsKey()
        */
        virtual Object::Holder get(Object::View vKey) const = 0;

        /**
        * Return the value to which this map maps the specified key. Return
        * <tt>NULL</tt> if the map contains no mapping for this key. A return
        * value of <tt>NULL</tt> does not <i>necessarily</i> indicate that
        * the map contains no mapping for the key; it's also possible that
        * the map explicitly maps the key to <tt>NULL</tt>.
        * The <tt>containsKey</tt> operation may be used to distinguish these
        * two cases.
        *
        * @param vKey key whose associated value is to be returned.
        *
        * @return the value to which this map maps the specified key, or
        *         <tt>NULL</tt> if the map contains no mapping for this key.
        *
        * @see #containsKey()
        */
        virtual Object::Holder get(Object::View vKey) = 0;

        /**
        * Associate the specified value with the specified key in this map.
        * If the map previously contained a mapping for this key, the old
        * value is replaced by the specified value.
        *
        * @param vKey key with which the specified value is to be associated.
        * @param ohValue value to be associated with the specified key.
        *
        * @return previous value associated with specified key, or
        *         <tt>NULL</tt> if there was no mapping for key.  A
        *         <tt>NULL</tt> return can also indicate that the map
        *         previously associated <tt>NULL</tt> with the specified key.
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the #put() operation is not supported by this map.
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue) = 0;

        /**
        * Remove the mapping for this key from this map if it is present.
        *
        * Return the value to which the map previously associated the key, or
        * <tt>NULL</tt> if the map contained no mapping for this key.  (A
        * <tt>NULL</tt> return can also indicate that the map previously
        * associated <tt>NULL</tt> with the specified key.)  The map will not
        * contain a mapping for the specified  key once the call returns.
        *
        * @param vKey key whose mapping is to be removed from the map.
        *
        * @return previous value associated with specified key, or <tt>NULL</tt>
        *         if there was no mapping for key.
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the #remove() operation is not supported by this map.
        */
        virtual Object::Holder remove(Object::View vKey) = 0;

        /**
        * Copy all of the mappings from the specified map to this map.
        * The effect of this call is equivalent to that of calling
        * {@link #put() put(k, v)} on this map once for each mapping from
        * key <tt>k</tt> to value <tt>v</tt> in the specified map.  The
        * behavior of this operation is unspecified if the specified map is
        * modified while the operation is in progress.
        *
        * @param vMap mappings to be stored in this map.
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the #put() operation is not supported by this map.
        */
        virtual void putAll(Map::View vMap) = 0;

        /**
        * Remove all mappings from this map.
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the #clear()operation is not supported by this map.
        */
        virtual void clear() = 0;

        /**
        * Return a set of the keys contained in this map.  The set is backed
        * by the map, so changes to the map are reflected in the
        * set. If the map is modified while an iteration over the set is in
        * progress, the results of the iteration are undefined.
        *
        * @return a set of the keys contained in this map.
        */
        virtual Set::View keySet() const = 0;

        /**
        * Return a set of the keys contained in this map.  The set is backed
        * by the map, so changes to one are reflected in the
        * other. If the map is modified while an iteration over the set is in
        * progress, the results of the iteration are undefined.
        *
        * @return a set of the keys contained in this map.
        */
        virtual Set::Handle keySet() = 0;

        /**
        * Return a collection of the values contained in this map. The
        * collection is backed by the map, so changes to the map are
        * reflected in the set. If the map is modified while an
        * iteration over the collection is in progress, the results of the
        * iteration are undefined.
        *
        * @return a collection of the values contained in this map.
        */
        virtual Collection::View values() const = 0;

        /**
        * Return a collection of the values contained in this map. The
        * collection is backed by the map, so changes to one are
        * reflected in the other. If the map is modified while an
        * iteration over the collection is in progress, the results of the
        * iteration are undefined.
        *
        * @return a collection of the values contained in this map.
        */
        virtual Collection::Handle values() = 0;

        /**
        * Return a set of the mappings contained in this map. Each element in
        * the returned set is a {@link Map::Entry::View}.  The set is backed
        * by the map, so changes to the map are reflected in the set. If the
        * map is modified while an iteration over the set is in progress, the
        * results of the iteration are undefined.
        *
        * @return a set of the mappings contained in this map.
        */
        virtual Set::View entrySet() const = 0;

        /**
        * Return a set of the mappings contained in this map. Each element in
        * the returned set is a {@link Map::Entry::Handle}.  The set is
        * backed by the map, so changes to one are reflected in the other. If
        * the map is modified while an iteration over the set is in progress,
        * the results of the iteration are undefined.
        *
        * @return a set of the mappings contained in this map.
        */
        virtual Set::Handle entrySet() = 0;

        // ----- Defaultable methods ----------------------------------------

        /**
        * Returns the value to which the specified key is mapped, or
        * ohDefaultValue if this map contains no mapping for the key.
        *
        * The default implementation makes no guarantees about
        * synchronization or atomicity properties of this method. Any
        * implementation providing atomicity guarantees must override this
        * method and document its concurrency properties.
        *
        * @param vKey            key with which the specified value is to be associated.
        * @param ohDefaultValue  value to be associated with the specified key.
        *
        * @return the value to which the specified key is mapped, or
        *         ohDefaultValue if this map contains no mapping for the key
        *
        * @since 12.2.1
        */
        virtual Object::Holder getOrDefault(Object::View vKey,
                Object::Holder ohDefaultValue) const
            {
            Object::Holder oh;
            return (((oh = get(vKey)) != NULL) || containsKey(vKey))
                ? oh
                : ohDefaultValue;
            }

        /**
        * If the specified key is not already associated with a value (or
        * is mapped to NULL) associates it with the given value and returns NULL,
        * else returns the current value.
        *
        * The default implementation makes no guarantees about synchronization
        * or atomicity properties of this method. Any implementation providing atomicity
        * guarantees must override this method and document its concurrency properties.
        *
        * @param vKey     key with which the specified value is to be associated.
        * @param ohValue  value to be associated with the specified key.
        *
        * @return the previous value associated with the specified key, or NULL
        *         if there was no mapping for the key. (A NULL return can also indicate
        *         that the map previously associated NULL with the key, if the
        *         implementation supports NULL values.)
        *
        * @since 12.2.1
        */
        virtual Object::Holder putIfAbsent(Object::View vKey, Object::Holder ohValue)
            {
            Object::Holder oh = get(vKey);
            if (oh == NULL)
                {
                oh = put(vKey, ohValue);
                }

            return oh;
            }

        /**
        * Removes the entry for the specified key only if it is currently
        * mapped to the specified value.
        *
        * The default implementation makes no guarantees about synchronization
        * or atomicity properties of this method. Any implementation providing
        * atomicity guarantees must override this method and document its concurrency properties.
        *
        * @param vKey    key associated with the specified value to be removed.
        * @param vValue  value to be removed, if it is associated with the specified key.
        *
        * @return true if the value was removed
        *
        * @since 12.2.1
        */
        virtual bool remove(Object::View vKey, Object::View vValue)
            {
            Object::View vCurValue = get(vKey);
            if (!equals(vCurValue, vValue) ||
                    (vCurValue == NULL && !containsKey(vKey)))
                {
                return false;
                }
            remove(vKey);
            return true;
            }

        /**
        * Replaces the entry for the specified key only if currently mapped to
        * the specified value.
        *
        * The default implementation does not throw NullPointerException for maps that
        * do not support NULL values if oldValue is NULL unless newValue is also NULL.
        *
        * The default implementation makes no guarantees about synchronization or
        * atomicity properties of this method. Any implementation providing atomicity
        * guarantees must override this method and document its concurrency properties.
        *
        * @param vKey        key associated with the specified value to be replace.
        * @param vOldValue   value to be replaced, if it is associated with the specified key.
        * @param ohNewValue  value to replace old value, if old value is
        *                    associated with the specified key.
        *
        * @return true if the value was replaced
        *
        * @since 12.2.1
        */
        virtual bool replace(Object::View vKey, Object::View vOldValue, Object::Holder ohNewValue)
            {
            Object::View vCurValue = get(vKey);
            if (!equals(vCurValue, vOldValue) ||
                    (vCurValue == NULL && !containsKey(vKey)))
                {
                return false;
                }
            put(vKey, ohNewValue);
            return true;
            }

        /**
        * Replaces the entry for the specified key only if it is currently mapped to some value.
        *
        * The default implementation makes no guarantees about synchronization or
        * atomicity properties of this method. Any implementation providing atomicity
        * guarantees must override this method and document its concurrency properties.
        *
        * @param vKey    key associated with the specified value to be replace.
        * @param vValue  new value used to replace, if any value is associated
        *                with the specified key.
        *
        * @return the previous value associated with the specified key, or NULL if
        *         there was no mapping for the key. (A NULL return can also indicate that
        *         the map previously associated NULL with the key, if the implementation
        *         supports NULL values.)
        *
        * @since 12.2.1
        */
        virtual Object::Holder replace(Object::View vKey, Object::Holder ohValue)
            {
            Object::Holder ohCurValue;
            if (((ohCurValue = get(vKey)) != NULL) || containsKey(vKey))
                {
                ohCurValue = put(vKey, ohValue);
                }
            return ohCurValue;
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_HPP
