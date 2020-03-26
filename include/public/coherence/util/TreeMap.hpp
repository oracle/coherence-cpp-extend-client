/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TREE_MAP_HPP
#define COH_TREE_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/NavigableMap.hpp"
#include "coherence/util/SortedMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A tree map implementation. Stored as an AVL tree. Implementation is based on
* the public domain implementation by Julienne Walker. This implementation is
* not thread-safe.
*
* @see <a href="http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_avl.aspx">
*      Implementation by Julienne Walker</a>
*
* @author tb  2009.02.22
*/
class COH_EXPORT TreeMap
    : public cloneable_spec<TreeMap,
        extends<AbstractMap>,
        implements<NavigableMap> >
    {
    friend class factory<TreeMap>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        TreeMap();

        /**
        * @internal
        */
        TreeMap(Comparator::View vComparator);

        /**
        * @internal
        */
        TreeMap(const TreeMap& that);

        /**
        * @internal
        */
        virtual ~TreeMap();


    // ----- Map interface --------------------------------------------------

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
        virtual bool containsKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        using Map::get;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet();


    // ----- SortedMap interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Comparator::View comparator() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View firstKey() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View lastKey() const;

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle headMap(Object::View vToKey);

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View headMap(Object::View vToKey) const;

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle subMap(Object::View vFromKey,
                Object::View vToKey);

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View subMap(Object::View vFromKey,
                Object::View vToKey) const;

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle tailMap(Object::View vFromKey);

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View tailMap(Object::View vFromKey) const;


    // ----- NavigableMap interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View ceilingKey(Object::View vKey) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::View floorKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View higherKey(Object::View vKey) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::View lowerKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::Holder pollFirstEntry();

        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::Holder pollLastEntry();

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle headMap(Object::View vToKey, bool toInclusive);

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View headMap(Object::View vToKey, bool toInclusive) const;

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle subMap(Object::View vFromKey, bool fromInclusive,
                Object::View vToKey, bool toInclusive);

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View subMap(Object::View vFromKey, bool fromInclusive,
                Object::View vToKey, bool toInclusive) const;

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle tailMap(Object::View vFromKey, bool fromInclusive);

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View tailMap(Object::View vFromKey, bool fromInclusive) const;


        // ----- inner class: Node ----------------------------------------------

    public:
        /**
        * An AVL tree node. This class is used only within the
        * TreeMap class and its derivations.
        */
        class COH_EXPORT Node
            : public cloneable_spec<Node,
                extends<Object>,
                implements<Map::Entry> >
        {
        friend class factory<Node>;

        // ----- constructors -------------------------------------------

        protected:
            /**
            * @internal
            */
            Node(Object::View ovKey);

            /**
            * @internal
            */
            Node(const Node& that);

        // ----- Node interface -----------------------------------------

        protected:
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
            virtual Object::Holder getValue() const;

            /**
            * Set the value associated with the node.
            *
            * @param ohValue the value assocaited with the node
            *
            * @return the old value associated with the node
            */
            virtual Object::Holder setValue(Object::Holder ohValue);

            /**
            * Determine if this node is a part of a 2-3-4 leaf node
            * (i.e. at least one NULL child).
            *
            * @return true if this node is a leaf
            */
            virtual bool isLeaf() const;

            /**
            * Unlink this element and all sub elements.
            */
            virtual void dissolve();

        // ----- Map::Entry interface -----------------------------------

        public:
            /**
            * Return the key corresponding to this entry.
            *
            * @return the key corresponding to this entry
            */
            virtual Object::View getKey() const;

            /**
            * Return the value corresponding to this entry.
            *
            * @return the value corresponding to this entry
            */
            virtual Object::Holder getValue();

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
            FinalView<Object> f_vKey;

            /**
            * The value of the node.
            */
            MemberHolder<Object>  m_ohValue;

            /**
            * The AVL balance factor of the sub-tree.
            */
            int32_t nBalance;

            /**
            * The parent of this node.
            */
            MemberHandle<Node> m_hParent;

            /**
            * The left child of this node.
            */
            MemberHandle<Node> m_hLeft;

            /**
            * The right child of this node.
            */
            MemberHandle<Node> m_hRight;

        // ----- friends --------------------------------------------

        friend class TreeMap;
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
        virtual void removeNode(Node::Handle hNode);

        /**
        * Determine if Node is the head of the tree.
        *
        * @param vNode  the node to compare to the head of the tree
        *
        * @return true if the passed in Node is the head of the tree
        */
        virtual bool isHead(Node::View vNode) const;

        /**
        * Return an Iterator over this tree map.
        *
        * @return an Iterator over this tree map
        */
        virtual Iterator::Handle iterator() const;

        /**
        * Return an Iterator over this tree map.
        *
        * @return an Iterator over this tree map
        */
        virtual Muterator::Handle iterator();

        /**
        * Compare two Objects. If both Objects are comparable with this tree
        * map's Comparator, return < 0, 0 or > 0 if the first Object is less
        * than, equal to, or greater than the second Object.
        *
        * @param v1  the first object to compare
        * @param v2  the second object to compare
        *
        * @return  < 0, 0 or > 0 if the first Object is less than,
        * equal to, or greater than the second Object
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const;

        /**
        * Find the specified key and return its node.
        *
        * @param vKey  the key to look for
        *
        * @return the node containing the index or NULL if the index is not
        *         in the TreeMap
        */
        virtual Node::Handle find(Object::View vKey) const;

        /**
        * Get the head node.
        *
        * @return this tree map's head node
        */
        virtual Node::Handle getHeadNode() const;

    protected:

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
        using Map::replace;

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
        * If the tree is empty then null is returned. If the index already
        * exists then the existing Node is returned, otherwise the Node which
        * will be the parent of the new Node is returned.
        *
        * @param ovKey  the key of the new node
        *
        * @return NULL, node, or parent
        */
        virtual Node::Handle findInsertionPoint(Object::View ovKey) const;

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
        * @param vKey     the key of the new Node
        * @param ohValue  the value of the new Node
        *
        * @return the newly instantiated Node
        */
        virtual Node::Handle instantiateNode(Object::View vKey,
                Object::Holder ohValue);


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

        /**
        * The comparator used to maintain order in this tree map, or
        * NULL if it uses the natural ordering of its keys.
        */
        FinalView<Comparator> f_vComparator;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TREE_MAP_HPP
