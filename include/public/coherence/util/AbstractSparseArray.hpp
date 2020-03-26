/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_SPARSE_ARRAY_HPP
#define COH_ABSTRACT_SPARSE_ARRAY_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractLongArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A data structure resembling an array indexed by long values, stored as an
* AVL tree. Implementation is based on the public domain implementation by
* Julienne Walker. This implementation is not thread-safe.
*
* @see <a href="http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_avl.aspx">
*      Implementation by Julienne Walker</a>
*
* @author js  2008.04.25
*/
class COH_EXPORT AbstractSparseArray
    : public abstract_spec<AbstractSparseArray,
        extends<AbstractLongArray> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractSparseArray();

        /**
        * @internal
        */
        AbstractSparseArray(const AbstractSparseArray& that);

        /**
        * @internal
        */
        virtual ~AbstractSparseArray();


    // ----- LongArray interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(int64_t lIndex, Object::Holder ohValue);

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
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual size32_t getSize() const;

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator();

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator() const;

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex);

        /**
        * {@inheritDoc}
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getFirstIndex() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getLastIndex() const;


    // ----- inner class: Node ----------------------------------------------

    public:
        /**
        * An AVL tree node. This class is used only within the
        * AbstractSparseArray class and its derivations.
        */
        class COH_EXPORT Node
            : public abstract_spec<Node>
        {
        // ----- constructors -------------------------------------------

        protected:
            /**
            * @internal
            */
            Node(int64_t lKey);

            /**
            * @internal
            */
            Node(const Node& that);


        // ----- Node interface -----------------------------------------

        public:
            /**
            * Adopt a child node
            *
            * @param hChild  the child to adopt
            * @param fLeft   the position of the child
            */
            virtual void adopt(Node::Handle hChild, bool fLeft);

            /**
            * Get the value associated with the node.
            *
            * @return the value associated with the node.
            */
            virtual Object::Holder getValue() const = 0;

            /**
            * Set the value associated with the node.
            *
            * @param ohValue the value assocaited with the node
            *
            * @return the old value associated with the node
            */
            virtual Object::Holder setValue(Object::Holder ohValue) = 0;

            /**
            * Determine if this node is a part of a 2-3-4 leaf node
            * (i.e. at least one NULL child).
            *
            * @return true if this node is a leaf
            */
            virtual bool isLeaf() const;

            /**
            * Return true iff the node is linked to other nodes.
            *
            * @return true iff the node has a parent or children
            */
            virtual bool isLinked() const;

            /**
            * Unlink this element and all sub elements.
            */
            virtual void dissolve();

        // ----- Object interface ---------------------------------------

        public:
            /**
            * {@inheritDoc}
            */
            virtual TypedHandle<const String> toString() const;

            /**
            * {@inheritDoc}
            */
            virtual void onInit();

        // ----- data members -------------------------------------------

        public:
            /**
            * The key of the node. The key, once set, is considered
            * immutable.
            */
            int64_t key;

            /**
            * The AVL balance factor of the sub-tree.
            */
            int32_t balance;

            /**
            * The parent of this node.
            */
            MemberHandle<Node> parent;

            /**
            * The left child of this node.
            */
            MemberHandle<Node> left;

            /**
            * The right child of this node.
            */
            MemberHandle<Node> right;
        };


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Remove the specified node from the tree.
        *
        * The supplied node must be part of the tree.
        *
        * @param hNode  the node to remove
        */
        virtual void remove(Node::Handle hNode);

        /**
        * Determine if Node is the head of the tree.
        *
        * @param vNode  the node to compare to the head of the tree
        *
        * @return true if the passed in Node is the head of the tree
        */
        virtual bool isHead(Node::View vNode) const;

    protected:
        /**
        * Find the specified key and return its node.
        *
        * @param lIndex  the long index to look for in the SparseArray
        *
        * @return the node containing the index or NULL if the index is not
        *         in the SparseArray
        */
        virtual Node::Handle find(int64_t lIndex) const;

        /**
        * Replace one node with another.
        *
        * @param hNodeA  the node to be unlinked
        * @param hNodeB  the node to be linked in nodeA's place; may be NULL
        *
        * @return nodeB's new parent
        */
        virtual Node::Handle replace(Node::Handle hNodeA,
                Node::Handle hNodeB);

        /**
        * Rotate a node in a given direction.
        *
        * @param hNode  the node to rotate
        * @param fLeft  the rotation direction
        *
        * @return the node's new parent (former child)
        */
        virtual Node::Handle rotate(Node::Handle hNode, bool fLeft);

        /**
        * Double rotate a node in a given direction.
        *
        * @param hNode  the node to rotate
        * @param fLeft  the final rotation direction
        *
        * @return the node's new parent (former grand child)
        */
        virtual Node::Handle doubleRotate(Node::Handle hNode,
                bool fLeft);

        /**
        * Adjust the balance factor of a node and its descendants prior to a
        * double rotation.
        *
        * @param hNode   the node which was rotated
        * @param hChild  the child to adjust
        * @param nBal    the balance adjustment
        */
        virtual void adjustDoubleBalance(Node::Handle hNode,
                Node::Handle hChild, int32_t nBal);

        /**
        * Find the point at which a Node with the specified index would be
        * inserted.
        *
        * If the tree is empty then NULL is returned. If the index already
        * exists then the existing Node is returned, otherwise the Node which
        * will be the parent of the new Node is returned.
        *
        * @param lIndex  the index of the new node
        *
        * @return NULL, node, or parent
        */
        virtual Node::Handle findInsertionPoint(int64_t lIndex) const;

        /**
        * Insert a node into a tree and re-balance.
        *
        * @param hParent  the location at which to insert the node
        * @param hChild   the node to insert
        */
        virtual void balancedInsertion(Node::Handle hParent,
                Node::Handle hChild);

        /**
        * Re-balance the tree following the removal of a node.
        *
        * @param hPruned      the node whose sub-tree shrunk
        * @param fPrunedLeft  the side on which the sub-tree shrunk
        */
        virtual void balancePostRemove(Node::Handle hPruned,
                bool fPrunedLeft);

        /**
        * Create a new Node
        *
        * @param lKey     the key of the new Node
        * @param ohValue  the value of the new Node
        *
        * @return the newly instantiated Node
        */
        virtual Node::Handle instantiateNode(int64_t lKey,
                Object::Holder ohValue) = 0;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of nodes in the tree. This can be determined by
        * iterating through the tree, but by keeping the size cached, certain
        * operations that need the size of the tree up front are much more
        * efficient.
        */
        size32_t m_nSize;

        /**
        * The first node of the tree (or NULL if the tree is empty).  The
        * first node is referred to as the "head" or the "root" node.
        */
        mutable MemberHandle<Node> m_hHead;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_SPARSE_ARRAY_HPP
