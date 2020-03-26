/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUB_SET_HPP
#define COH_SUB_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Implements a set which is based on another set, which is assumed to be
* immutable. Unlike DeltaSet, the SubSet is assumed to be a subset of the
* underlying set, and optimizes for both remove and retain operations.
*
* @author tb 2009.02.10
*/
class COH_EXPORT SubSet
    : public cloneable_spec<SubSet,
        extends<AbstractSet> >
    {
    friend class factory<SubSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct this set based on an existing set.
        *
        * @param hSet  the set to base this subset on
        */
        SubSet(Set::Holder hSet);

        /**
        * Copy constructor.
        */
        SubSet(const SubSet& that);


    // ----- SubSet accessors -----------------------------------------------

    public:
        /**
        * Determine what items were in the original set.
        *
        * @return the set used to construct this SubSet
        */
        virtual Set::View getOriginal() const;

        /**
        * Determine if the set has been modified.
        *
        * @return  true if any items have been removed
        */
        virtual bool isModified() const;

        /**
        * Determine if the SubSet is tracking retained items versus removed
        * items.
        *
        * @return true if the SubSet is tracking just the retained items,
        *         false if the SubSet is tracking just the removed items
        */
        virtual bool isTrackingRetained() const;

        /**
        * Determine what items were added to the subset. Do not modify the
        * returned set.
        *
        * @return a set of retained items
        */
        virtual Set::View getRetained() const;

        /**
        * Determine if the SubSet is tracking removed items versus retained
        * items.
        *
        * @return true if the SubSet is tracking just the removed items,
        *         false if the SubSet is tracking just the retained items
        */
        virtual bool isTrackingRemoved() const;

        /**
        * Determine what items were removed from the subset.
        *
        * @return an immutable set of removed items
        */
        virtual Set::View getRemoved() const;

    protected:
        /**
         * Instantiate a new modification set containing either removed or
         * retained items.
         *
         * @param cSize  an initial size of the modification set
         */
        virtual Set::Handle instantiateModificationSet(size32_t cSize) const;

    public:
        /**
        * Get a mutable set of items that are retained in the subset.
        *
        * @return a mutable set of retained items
        */
        virtual Set::Handle ensureRetained();

        /**
        * Get a mutable set of items that are removed in the subset.
        *
        * @return a mutable set of removed items
        */
        virtual Set::Handle ensureRemoved();

        /**
        * Apply the changes to the underlying set ("commit").
        */
        virtual void resolve();

        /**
        * Discard the changes to the set ("rollback").
        */
        virtual void reset();


    // ----- Collection interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsAll(Collection::View vCol) const;

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
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hao = NULL) const;

        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual bool removeAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual bool retainAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual void clear();


    // ----- helpers --------------------------------------------------------

    protected:
        /**
         * Instantiate a new retained set with all elements in the specified
         * collection that also exist in the specified set and are not excluded.
         *
         * @param vColOuter    collection to iterate
         * @param vSetMatch    set to test the presence for each iterated element
         * @param vSetExclude  optional set of excluded elements
         */
     void retainAllInternal(Collection::View vColOuter, 
             Set::View vSetMatch, Set::View vSetExclude);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying set (assumed immutable).
        */
        FinalHolder<Set> f_ohSetOrig;

        /**
        * The removed or retained items.
        */
        MemberHandle<Set> m_hSetMod;

        /**
        * Toggles between whether the modifications are removed or retained.
        */
        bool m_fRetained;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SUB_SET_HPP
