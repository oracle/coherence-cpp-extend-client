/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DELTA_SET_HPP
#define COH_DELTA_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Implements a set which is based on another set, which is assumed to be
* immutable.
*
* @author tb 2009.02.10
*/
class COH_EXPORT DeltaSet
    : public class_spec<DeltaSet,
        extends<AbstractSet> >
    {
    friend class factory<DeltaSet>;

    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Construct this set based on an existing set.
        *
        * @param set  the set to base this delta set on
        */
        DeltaSet(Set::Holder hSet);


    // ----- DeltaSet accessors ---------------------------------------------

    public:
        /**
        * Determine what items were in the original set.
        *
        * @return the set used to construct this DeltaSet
        */
        virtual Set::View getOriginal() const;

        /**
        * Determine if the set has been modified.
        *
        * @return  true if any items have been removed
        */
        virtual bool isModified() const;

        /**
        * Determine what items were added to the DeltaSet.
        *
        * @return a set of added items
        */
        virtual Set::View getAdded() const;

        /**
        * Determine what items were removed from the DeltaSet.
        *
        * @return an immutable set of removed items
        */
        virtual Set::View getRemoved() const;

        /**
        * Get a mutable set of items that were added to the DeltaSet.
        *
        * @return a mutable set of added items
        */
        virtual Set::Handle ensureAdded();

        /**
        * Get a mutable set of items that are removed in the DeltaSet.
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


    // ----- Set interface --------------------------------------------------

    public:
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
        virtual ObjectArray::Handle toArray(ObjectArray::Handle hao = NULL) const;

        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual void clear();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying set (assumed immutable).
        */
        FinalHolder<Set> f_ohSetOrig;

        /**
        * The added items.
        */
        MemberHandle<Set> m_hSetAdded;

        /**
        * The removed items.
        */
        MemberHandle<Set> m_hSetRemoved;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DELTA_SET_HPP
