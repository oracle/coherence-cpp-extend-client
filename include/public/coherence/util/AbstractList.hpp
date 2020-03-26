/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_LIST_HPP
#define COH_ABSTRACT_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractCollection.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* This class provides a skeletal implementation of the List interface to
* minimize the effort required to implement this interface.
*/
class COH_EXPORT AbstractList
    : public abstract_spec<AbstractList,
        extends<AbstractCollection>,
        implements<List> >
    {
    protected:
        /**
        * @internal
        */
        AbstractList();

        /**
        * @internal
        */
        AbstractList(const AbstractList& that);


    // ----- List interface -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(size32_t i);

        /**
        * {@inheritDoc}
        */
        using List::get;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The cached hash of all the array elements. The hash code is not
        * cached until the List becomes immutable. A value of 0 indicates
        * that the has has not been cached.
        */
        mutable size32_t m_nHashCode;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_LIST_HPP
