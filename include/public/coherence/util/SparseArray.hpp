/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SPARSE_ARRAY_HPP
#define COH_SPARSE_ARRAY_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractSparseArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A data structure resembling an array keyed by long values.
*
* @author js 2008.04.08
*/
class COH_EXPORT SparseArray
    : public cloneable_spec<SparseArray,
        extends<AbstractSparseArray> >
    {
    friend class factory<SparseArray>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SparseArray.
        */
        SparseArray();

        /**
        * Copy constructor.
        */
        SparseArray(const SparseArray& that);

    // ----- LongArray interface --------------------------------------------

    public:
        using super::remove;


    // ----- AbstractSparseArray interface ----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove(AbstractSparseArray::Node::Handle hNode);

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractSparseArray::Node::Handle instantiateNode(
                int64_t lKey, Object::Holder ohValue);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SPARSE_ARRAY_HPP
