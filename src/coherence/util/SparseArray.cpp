/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SparseArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(SparseArray);


// ----- local class: ObjectNode --------------------------------------------

/**
* Concrete Node implementation.
*/
class ObjectNode
    : public cloneable_spec<ObjectNode,
        extends<AbstractSparseArray::Node> >
    {
    friend class factory<ObjectNode>;

    // ----- constructor ------------------------------------------------

    protected:
        /**
        * Create a new ObjectNode instance.
        *
        * @param lKey     key of the node
        * @param ohValue  value of the node
        */
        ObjectNode(int64_t lKey, Object::Holder ohValue)
            : super(lKey),
              m_ohValue(self(), ohValue)
            {
            }

        /**
        * @internal
        */
        ObjectNode(const ObjectNode& that)
            : super(that),
              m_ohValue(self(), that.m_ohValue)
            {
            }

    // ----- Node interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue() const
            {
            return m_ohValue;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder ohValue)
            {
            Object::Holder ohOldValue = m_ohValue;
            m_ohValue = ohValue;
            return ohOldValue;
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The value of the node.
        */
        MemberHolder<Object> m_ohValue;
    };


// ----- constructors -------------------------------------------------------

SparseArray::SparseArray()
    {
    }

SparseArray::SparseArray(const SparseArray& that)
    : super(that)
    {
    }


// ----- AbstractSparseArray methods ----------------------------------------

void SparseArray::remove(AbstractSparseArray::Node::Handle hNode)
    {
    super::remove(hNode);
    hNode->setValue(NULL);
    }

AbstractSparseArray::Node::Handle SparseArray::instantiateNode(int64_t lKey,
                Object::Holder ohValue)
    {
    return ObjectNode::create(lKey, ohValue);
    }

COH_CLOSE_NAMESPACE2
