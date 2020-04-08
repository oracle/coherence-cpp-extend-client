/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_SET_HPP
#define COH_ABSTRACT_SET_HPP

#include "coherence/lang.ns"
#include "coherence/util/AbstractCollection.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
 * This class provides a skeletal implementation of the <tt>Set</tt>
 * interface to minimize the effort required to implement this
 * interface.
 *
 * The process of implementing a set by extending this class is identical
 * to that of implementing a Collection by extending AbstractCollection,
 * except that all of the methods and constructors in subclasses of this
 * class must obey the additional constraints imposed by the <tt>Set</tt>
 * interface (for instance, the <tt>add</tt> method must not permit addition
 * of multiple instances of an object to a set).
 *
 * @author js 2008.05.12
 */
class COH_EXPORT AbstractSet
    : public abstract_spec<AbstractSet,
        extends<AbstractCollection>,
        implements<Set> >
    {
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
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_SET_HPP

