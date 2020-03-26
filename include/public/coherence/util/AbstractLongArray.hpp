/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_LONG_ARRAY_HPP
#define COH_ABSTRACT_LONG_ARRAY_HPP

#include "coherence/lang.ns"

#include "coherence/util/LongArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Abstract base class for LongArray implementations.
*
* @author js  2008.04.04
*/
class COH_EXPORT AbstractLongArray
    : public abstract_spec<AbstractLongArray,
        extends<Object>,
        implements<LongArray> >
    {
    // ----- LongArray interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t add(Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual bool exists(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(int64_t lIndex);

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View vElement) const;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getSize() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getFirstIndex() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getLastIndex() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_LONG_ARRAY_HPP
