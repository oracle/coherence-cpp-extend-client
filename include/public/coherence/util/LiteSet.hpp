/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LITE_SET_HPP
#define COH_LITE_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/Enumeration.hpp"
#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An implementation of coherence::util::Set that is optimal (in terms of both
* size and speed) for very small sets of data but still works excellently
* with large sets of data.  This implementation is not thread-safe.
*
* The LiteSet implementation switches at runtime between several different
* sub-implementations for storing the set of objects, described here:
*
* <ol>
* <li>"empty set" - a set that contains no data;
* <li>"single entry" - a reference directly to an item is used to represent
*     a set with exactly one item in it;
* <li>"ObjectArray" - a reference is held to an array of Objects that store
*     the contents of the Set; the item limit for this implementation is
*     determined by the threshold constant;
* <li>"delegation" - for more than threshold items, a set is created to
*     delegate the set management to; sub-classes can override the default
*     delegation class (HashSet) by overriding the factory method
*     #instantiateSet().
* </ol>
*
* The LiteSet implementation supports the NULL value.
*
* @author cp/lh 10/18/2010
*/
class COH_EXPORT LiteSet
    : public cloneable_spec<LiteSet,
        extends<AbstractSet> >
    {
    friend class factory<LiteSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a LiteSet
        */
        LiteSet();

        /**
        * Construct a LiteSet containing the elements of the passed Collection.
        *
        * @param vCollection  a Collection
        */
        LiteSet(Collection::View vCollection);

        /*
        * Copy constructor
        *
        * @param that  the LiteSet instance to copy from
        */
        LiteSet(const LiteSet& that);


    // ----- Set interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const;

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator();

        /**
        * {@inheritDoc}
        */
        ObjectArray::Handle toArray(ObjectArray::Handle haDest = NULL) const;

        /**
        * {@inheritDoc}
        */
        bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        bool remove(Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual bool containsAll(Collection::View vCollection) const;

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(Collection::View vCollection);

        /**
        * {@inheritDoc}
        */
        virtual bool retainAll(Collection::View vCollection);

        /**
        * {@inheritDoc}
        */
        virtual bool removeAll(Collection::View vCollection);

        /**
        * {@inheritDoc}
        */
        virtual void clear();


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * (Factory pattern) Instantiate a Set object to store items in once
        * the "lite" threshold has been exceeded. This method permits
        * inheriting classes to easily override the choice of the Set object.
        *
        * @return an instance of Set
        */
        Set::Handle instantiateSet();

        /**
        * (Factory pattern) Instantiate a Set object to store items in once
        * the "lite" threshold has been exceeded. This method permits
        * inheriting classes to easily override the choice of the Set object.
        *
        * @return an instance of Set
        */
        Set::View instantiateSet() const;

        /**
        * Initialize the contents of this Set from the passed array <tt>ao</tt>
        * containing <tt>c</tt> values.
        *
        * @param va  the array that contains the values to place in this Set
        * @param c   the number of values that will be placed into this Set
        */
        void initFromArray(ObjectArray::View va, size32_t c);

        /**
        * After a mutation operation has reduced the size of an underlying
        * Set, check if the delegation model should be replaced with a more
        * size-efficient storage approach, and switch accordingly.
        */
        void checkShrinkFromOther();

    private:
        /**
        * Scan up to the first <tt>c</tt> elements of the passed array
        * <tt>ao</tt> looking for the specified Object <tt>o</tt>. If it is
        * found, return its position <tt>i</tt> in the array such that
        * <tt>(0 &lt;= i &lt; c)</tt>. If it is not found, return <tt>npos</tt>.
        *
        * @param va  the array of objects to search
        * @param c   the number of elements in the array to search
        * @param v   the object to look for
        *
        * @return the index of the object, if found; otherwise npos
        */
        static size32_t indexOf(ObjectArray::View va, size32_t c, Object::View v);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
       static const size32_t npos = size32_t(-1);

    private:
        /**
        * The default point above which the LiteSet delegates to another set
        * implementation.
        */
        static const size32_t threshold = 8;

        /**
        * Implementation:  Empty set.
        */
        static const size32_t i_empty = 0;

        /**
        * Implementation:  Single-item set.
        */
        static const size32_t i_single = 1;

        /**
        * Implementation:  Array set of 1 item.
        */
        static const size32_t i_array_1 = 2;

        /**
        * Implementation:  Array set of 2 items.
        */
        static const size32_t i_array_2 = 3;

        /**
        * Implementation:  Array set of 3 items.
        */
        static const size32_t i_array_3 = 4;

        /**
        * Implementation:  Array set of 4 items.
        */
        static const size32_t i_array_4 = 5;

        /**
        * Implementation:  Array set of 5 items.
        */
        static const size32_t i_array_5 = 6;

        /**
        * Implementation:  Array set of 6 items.
        */
        static const size32_t i_array_6 = 7;

        /**
        * Implementation:  Array set of 7 items.
        */
        static const size32_t i_array_7 = 8;

        /**
        * Implementation:  Array set of 8 items.
        */
        static const size32_t i_array_8 = 9;

        /**
        * Implementation:  Delegation.
        */
        static const size32_t i_other = 10;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Implementation, one of i_empty, i_single, i_array_* or i_other.
        */
        octet_t m_nImpl;

        /**
        * The set contents, based on the implementation being used.
        */
        MemberHolder<Object> m_hContents;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LITE_SET_HPP
