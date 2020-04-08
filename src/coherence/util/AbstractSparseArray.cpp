/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SparseArray.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(AbstractSparseArray)

// ----- local class: Crawler -------------------------------------------

/**
* A tree node iterator.
*
* The methods of this local class are not synchronized; the enclosing
* class is responsible for synchronization.
*/
class COH_EXPORT Crawler
    : public class_spec<Crawler,
        extends<Object>,
        implements<LongArrayIterator> >
    {
    friend class factory<Crawler>;

    // ----- RelativePosition enum ----------------------------------

    public:
        enum RelativePosition
            {
            ABOVE,
            LEFT,
            SITTING,
            RIGHT
            };

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new Crawler.
        *
        * @param hHead    the first element of the iterator
        * @param fromDir  relative direction traversal is from
        * @param hArray   backing sparse array instance
        */
        Crawler(SparseArray::Node::Handle hHead,
                RelativePosition fromDir,
                AbstractSparseArray::Handle hArray)
            : m_hCurrent(self(), hHead, true),
              m_fromDir(fromDir),
              f_hArray(self(), hArray)
            {
            }

    // ----- LongArrayIterator interface ----------------------------

    public:
        virtual int64_t getIndex() const
            {
            return currentNode()->key;
            }

        virtual Object::Holder getValue() const
            {
            return currentNode()->getValue();
            }

        virtual Object::Holder setValue(Object::Holder ohValue)
            {
            return currentNode()->setValue(ohValue);
            }

        virtual void remove()
            {
            AbstractSparseArray::Node::Handle hNode = currentNode();
            if (hasNext())
                {
                // advance to the next node
                next();

                // pretend that the "current" node advanced to is actually
                // the "next current" node
                m_fromDir = LEFT;
                }

            if (hNode->isLinked() || f_hArray->isHead(hNode))
                {
                f_hArray->remove(hNode);
                }
            else
                {
                // if the node is not linked and not the head then it has
                // already been removed
                COH_THROW (IllegalStateException::create(
                        "the node has already been removed"));
                }
            }

    // ----- Iterator interface -------------------------------------

    public:
        virtual bool hasNext() const
            {
            if (NULL == m_hCurrent)
                {
                return false;
                }

            while (true)
                {
                switch (m_fromDir)
                    {
                    case ABOVE:
                        // try to crawl down to the left
                        if (m_hCurrent->left != NULL)
                            {
                            m_hCurrent = m_hCurrent->left;
                            break;
                            }
                        m_fromDir = LEFT;
                        // no break;

                    case LEFT:
                        // the current node is the next element to return
                        return true;

                    case SITTING:
                        // try to crawl down to the right
                        if (m_hCurrent->right != NULL)
                            {
                            m_fromDir  = ABOVE;
                            m_hCurrent = m_hCurrent->right;
                            break;
                            }
                        // no break;

                    case RIGHT:
                        // try to crawl up
                        if (m_hCurrent->parent != NULL)
                            {
                            m_fromDir = m_hCurrent ==
                                    m_hCurrent->parent->left ? LEFT : RIGHT;
                            m_hCurrent = cast
                                    <AbstractSparseArray::Node::Handle>
                                    (m_hCurrent->parent);
                            break;
                            }
                        // all out of nodes to crawl on
                        m_hCurrent = NULL;
                        return false;

                    default:
                        COH_THROW_STREAM(IllegalStateException,
                                "Invalid Direction: " << m_fromDir);
                    }
                }
            }

        virtual Object::Holder next()
            {
            return nextNode(false)->getValue();
            }

    // ----- Object interface ---------------------------------------

    public:
        virtual TypedHandle<const String> toString() const
            {
            switch (m_fromDir)
                {
                case ABOVE:
                    return COH_TO_STRING("crawled into " << m_hCurrent->key);
                case LEFT:
                    return COH_TO_STRING("returned to " << m_hCurrent->key <<
                            " from the left child");
                case SITTING:
                    return COH_TO_STRING("sitting in " << m_hCurrent->key);
                case RIGHT:
                    return COH_TO_STRING("returned to " << m_hCurrent->key <<
                            " from the right child");
                }

            COH_THROW_STREAM(IllegalStateException,
                    "invalid direction: " << m_fromDir);
            }


    // ----- helper methods -----------------------------------------

    protected:
        /**
        * Returns the next Node in the iteration.
        *
        * @param fPeek  true if performing peek operation, false
        *               otherwise
        *
        * @return the next Node in the iteration
        *
        * @throws NoSuchElementException iterator has no more elements
        */
        virtual AbstractSparseArray::Node::Handle nextNode(
                bool fPeek) const
            {
            if (NULL == m_hCurrent)
                {
                COH_THROW (NoSuchElementException::create());
                }

            // save the original state so that we can restore it
            SparseArray::Node::Handle hOrigCurrent = m_hCurrent;
            RelativePosition          origFromDir  = m_fromDir;

            while (true)
                {
                switch (m_fromDir)
                    {
                    case ABOVE:
                        // try to crawl down to the left
                        if (m_hCurrent->left != NULL)
                            {
                            m_hCurrent = m_hCurrent->left;
                            break;
                            }
                        // no break;

                    case LEFT:
                        {
                        // the current node is the next element to return
                        AbstractSparseArray::Node::Handle
                                hReturnVal = m_hCurrent;
                        if (fPeek)
                            {
                            // restore original state
                            m_fromDir  = origFromDir;
                            m_hCurrent = hOrigCurrent;
                            }
                        else
                            {
                            m_fromDir = SITTING;
                            }

                        return hReturnVal;
                        }
                    case SITTING:
                        // try to crawl down to the right
                        if (m_hCurrent->right != NULL)
                            {
                            m_fromDir = ABOVE;
                            m_hCurrent = m_hCurrent->right;
                            break;
                            }
                        // no break;

                    case RIGHT:
                        // try to crawl up
                        if (m_hCurrent->parent != NULL)
                            {
                            m_fromDir = m_hCurrent ==
                                    m_hCurrent->parent->left ? LEFT : RIGHT;
                            m_hCurrent = cast
                                    <AbstractSparseArray::Node::Handle>
                                    (m_hCurrent->parent);
                            break;
                            }

                        if (fPeek)
                            {
                            // restore original state
                            m_fromDir  = origFromDir;
                            m_hCurrent = hOrigCurrent;
                            }
                        else
                            {
                            // all out of nodes to crawl on
                            m_hCurrent = NULL;
                            }

                        COH_THROW (NoSuchElementException::create());

                    default:
                        if (fPeek)
                            {
                            // restore original state
                            m_fromDir  = origFromDir;
                            m_hCurrent = hOrigCurrent;
                            }

                        COH_THROW_STREAM(IllegalStateException,
                                "invalid direction: " << m_fromDir);
                    }
                }
            }

        AbstractSparseArray::Node::Handle currentNode() const
            {
            AbstractSparseArray::Node::Handle hNode = m_hCurrent;
            if (NULL == hNode || m_fromDir != SITTING)
                {
                COH_THROW (IllegalStateException::create());
                }
            return hNode;
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * Current Node
        */
        mutable MemberHandle<SparseArray::Node> m_hCurrent;

        /**
        * Traversal direction
        */
        mutable RelativePosition m_fromDir;

        /**
        * Handle to the SparseArray used for removal from iterator
        */
        FinalHandle<AbstractSparseArray> f_hArray;
    };


/**
* Iterator which throws an exception if an attempt is made to set or
* remove an element using the iterator.
*
*/
class ReadOnlyCrawler
    : public class_spec<ReadOnlyCrawler,
        extends<Crawler> >
    {
    friend class factory<ReadOnlyCrawler>;

    // ----- constructors -------------------------------------------

    protected:
        ReadOnlyCrawler(SparseArray::Node::Handle hHead,
                RelativePosition fromDir)
            : super(hHead, fromDir, (AbstractSparseArray::Handle) NULL)
            {
            }

    // ----- LongArrayIterator interface ----------------------------

    public:
        virtual void remove()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "Read only iterator"));
            }

        virtual Object::Holder setValue(Object::Holder)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "Read only iterator"));
            }
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructor --------------------------------------------------------

AbstractSparseArray::AbstractSparseArray()
    : m_nSize(0), m_hHead(self(), (AbstractSparseArray::Node::Handle) NULL, true)
    {
    }

AbstractSparseArray::AbstractSparseArray(const AbstractSparseArray& that)
    : super(that), m_nSize(that.m_nSize),
      m_hHead(self(), (AbstractSparseArray::Node::Handle) NULL, true)
    {
    if (that.m_hHead != NULL)
        {
        m_hHead = cast<AbstractSparseArray::Node::Handle>
                (that.m_hHead->clone());
        }
    }

AbstractSparseArray::~AbstractSparseArray()
    {
    clear(); // unlink or it will be leaked
    }

// ----- LongArray interface ------------------------------------------------

Object::Holder AbstractSparseArray::get(int64_t lIndex) const
    {
    AbstractSparseArray::Node::View vNode = find(lIndex);
    return (NULL == vNode ? NULL : vNode->getValue());
    }

Object::Holder AbstractSparseArray::set(int64_t lIndex,
        Object::Holder ohValue)
    {
    AbstractSparseArray::Node::Handle hNode = findInsertionPoint(lIndex);
    if (hNode != NULL && hNode->key == lIndex)
        {
        return hNode->setValue(ohValue);
        }
    balancedInsertion(hNode, instantiateNode(lIndex, ohValue));
    return NULL;
    }

bool AbstractSparseArray::exists(int64_t lIndex) const
    {
    return find(lIndex) != NULL;
    }

Object::Holder AbstractSparseArray::remove(int64_t lIndex)
    {
    AbstractSparseArray::Node::Handle hNode = find(lIndex);
    if (NULL == hNode)
        {
        return NULL;
        }
    Object::Holder ohValue = hNode->getValue();
    remove(hNode);
    return ohValue;
    }

void AbstractSparseArray::clear()
    {
    // unlink the tree elements, the tree does not employ WeakHandles as they
    // are somewhat costly, so instead we must ensure to unlink them
    Node::Handle hHead = m_hHead;
    if (hHead != NULL)
        {
        hHead->dissolve();
        }

    m_hHead = NULL;
    m_nSize = 0;
    }

size32_t AbstractSparseArray::getSize() const
    {
    return m_nSize;
    }

LongArrayIterator::Handle AbstractSparseArray::iterator()
    {
    return Crawler::create((Node::Handle) m_hHead, Crawler::ABOVE, this);
    }

LongArrayIterator::Handle AbstractSparseArray::iterator() const
    {
    return ReadOnlyCrawler::create((Node::Handle) m_hHead, Crawler::ABOVE);
    }

LongArrayIterator::Handle AbstractSparseArray::iterator(int64_t lIndex)
    {
    AbstractSparseArray::Node::Handle hClosest = NULL;
    AbstractSparseArray::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int64_t lCurrent = hCurrent->key;
        if (lIndex < lCurrent)
            {
            hClosest = hCurrent;
            hCurrent = hCurrent->left;
            }
        else if (lIndex > lCurrent)
            {
            hCurrent = hCurrent->right;
            }
        else
            {
            return Crawler::create(hCurrent, Crawler::LEFT, this);
            }
        }

    return Crawler::create(hClosest, Crawler::LEFT, this);
    }

LongArrayIterator::Handle AbstractSparseArray::iterator(int64_t lIndex) const
    {
    AbstractSparseArray::Node::Handle hClosest = NULL;
    AbstractSparseArray::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int64_t lCurrent = hCurrent->key;
        if (lIndex < lCurrent)
            {
            hClosest = hCurrent;
            hCurrent = hCurrent->left;
            }
        else if (lIndex > lCurrent)
            {
            hCurrent = hCurrent->right;
            }
        else
            {
            return ReadOnlyCrawler::create(hCurrent, Crawler::LEFT);
            }
        }

    return ReadOnlyCrawler::create(hClosest, Crawler::LEFT);
    }

int64_t AbstractSparseArray::getFirstIndex() const
    {
    AbstractSparseArray::Node::View vNodeCur = m_hHead;
    if (NULL == vNodeCur)
        {
        return -1;
        }

    AbstractSparseArray::Node::View vNodeNext = vNodeCur->left;
    while (vNodeNext != NULL)
        {
        vNodeCur  = vNodeNext;
        vNodeNext = vNodeCur->left;
        }

    return vNodeCur->key;
    }

int64_t AbstractSparseArray::getLastIndex() const
    {
    AbstractSparseArray::Node::View vNodeCur = m_hHead;
    if (NULL == vNodeCur)
        {
        return -1;
        }

    AbstractSparseArray::Node::View vNodeNext = vNodeCur->right;
    while (vNodeNext != NULL)
        {
        vNodeCur  = vNodeNext;
        vNodeNext = vNodeCur->right;
        }

    return vNodeCur->key;
    }

bool AbstractSparseArray::isHead(
        AbstractSparseArray::Node::View vNode) const
    {
    return Object::equals(m_hHead, vNode);
    }


// ----- Helper methods -----------------------------------------------------

AbstractSparseArray::Node::Handle AbstractSparseArray::find(
        int64_t lIndex) const
    {
    AbstractSparseArray::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int64_t lCurrent = hCurrent->key;
        if (lIndex < lCurrent)
            {
            hCurrent = hCurrent->left;
            }
        else if (lIndex > lCurrent)
            {
            hCurrent = hCurrent->right;
            }
        else // lIndex == lCurrent
            {
            return hCurrent;
            }
        }

    return NULL;
    }

void AbstractSparseArray::remove(AbstractSparseArray::Node::Handle hNode)
    {
    if (NULL == hNode->left || NULL == hNode->right)
        {
        // at most one child node is NULL, perform simple adoption;
        // node's parent replaces node with node's only child
        AbstractSparseArray::Node::Handle hChild  =
                NULL == hNode->left ? hNode->right : hNode->left;
        AbstractSparseArray::Node::Handle hParent = replace(hNode, hChild);

        if (hParent != NULL)
            {
            // parent's sub-tree shurnk
            balancePostRemove(hParent, hNode->key < hParent->key);
            }
        }
    else
        {
        // node has two children; node's in-order successor (heir) will
        // take node's place in the tree and adopt node's children;
        // heir's right (only) child must also be adopted by
        // heir's current parent, care needs to be taken when
        // heir->parent == node

        // find heir
        AbstractSparseArray::Node::Handle hHeir = hNode->right;
        while (hHeir->left != NULL)
            {
            hHeir = hHeir->left;
            }

        // heir will replace node; taking it's children and balance
        hHeir->balance = hNode->balance;

        // perform adoptions
        if (hHeir->parent == hNode)
            {
            // heir's parent is node; heir keeps it right (only) child,
            // and adopts node's left
            hHeir->adopt(hNode->left, true);

            // heir will take node's place but with shorter right tree
            replace(hNode, hHeir);
            balancePostRemove(hHeir, false);
            }
        else
            {
            // heir->parent != node, therefore heir == parent->left
            // heir's parent adopts heir's only child, replacing heir
            hHeir->parent->adopt(hHeir->right, true);

            // heir adopts node's children
            hHeir->adopt(hNode->left, true);
            hHeir->adopt(hNode->right, false);

            // node's parent's left tree shrunk
            AbstractSparseArray::Node::Handle hPruned = hHeir->parent;
            replace(hNode, hHeir);
            balancePostRemove(hPruned, true);
            }
        }

    // invalidate any Crawler which may be sitting on node
    hNode->parent = hNode->right = hNode->left = NULL;
    --m_nSize;
    }

AbstractSparseArray::Node::Handle AbstractSparseArray::replace(
        AbstractSparseArray::Node::Handle hNodeA,
        AbstractSparseArray::Node::Handle hNodeB)
    {
    AbstractSparseArray::Node::Handle hParent = hNodeA->parent;

    if (NULL == hParent)
        {
        m_hHead = hNodeB;
        }
    else if (hParent->left == hNodeA)
        {
        hParent->left = hNodeB;
        }
    else
        {
        hParent->right = hNodeB;
        }

    if (hNodeB != NULL)
        {
        hNodeB->parent = hParent;
        }

    return hParent;
    }

AbstractSparseArray::Node::Handle AbstractSparseArray::rotate(
        AbstractSparseArray::Node::Handle hNode, bool fLeft)
    {
    AbstractSparseArray::Node::Handle hParent = hNode->parent;
    AbstractSparseArray::Node::Handle hChild  =
            fLeft ? hNode->right : hNode->left;

    replace(hChild, fLeft ? hChild->left : hChild->right); // push grand up
    hChild->adopt(hNode, fLeft);                           // push node down

    hNode->parent = hParent;       // revert node's parent for replace
    replace(hNode, hChild);        // push child up
    return hNode->parent = hChild; // restore and return node's new parent
    }

AbstractSparseArray::Node::Handle AbstractSparseArray::doubleRotate(
        AbstractSparseArray::Node::Handle hNode, bool fLeft)
    {
    rotate(fLeft ? hNode->right : hNode->left, !fLeft); // rotate child
    return rotate(hNode, fLeft);                        // rotate node
    }

void AbstractSparseArray::adjustDoubleBalance(
        AbstractSparseArray::Node::Handle hNode,
        AbstractSparseArray::Node::Handle hChild, int32_t nBal)
    {
    AbstractSparseArray::Node::Handle hGrand =
            hChild == hNode->left ? hChild->right : hChild->left;

    if (hGrand->balance == 0)
        {
        hNode->balance = hChild->balance = 0;
        }
    else if (hGrand->balance == nBal)
        {
        hNode->balance  = -nBal;
        hChild->balance = 0;
        }
    else
        {
        hNode->balance  = 0;
        hChild->balance = nBal;
        }
    hGrand->balance = 0;
    }

AbstractSparseArray::Node::Handle AbstractSparseArray::findInsertionPoint(
        int64_t lIndex) const
    {
    AbstractSparseArray::Node::Handle hNode = m_hHead;
    if (NULL == hNode)
        {
        return NULL;
        }

    while (true)
        {
        int64_t lCurr = hNode->key;
        if (lIndex > lCurr)
            {
            if (NULL == hNode->right)
                {
                return hNode;
                }
            else
                {
                hNode = hNode->right;
                }
            }
        else if (lIndex < lCurr)
            {
            if (NULL == hNode->left)
                {
                return hNode;
                }
            else
                {
                hNode = hNode->left;
                }
            }
        else
            {
            return hNode;
            }
        }
    }

void AbstractSparseArray::balancedInsertion(
        AbstractSparseArray::Node::Handle hParent,
        AbstractSparseArray::Node::Handle hChild)
    {
    if (NULL == hParent)
        {
        m_hHead = hChild;
        m_nSize = 1;
        return; // done
        }
    else if (hChild->key < hParent->key)
        {
        hParent->adopt(hChild, true);
        hParent->balance -= 1;
        }
    else
        {
        hParent->adopt(hChild, false);
        hParent->balance += 1;
        }
    ++m_nSize;

    // walk from the new child node up towards the head, terminating
    // once the sub-trees have been sufficiently balanced
    while (true)
        {
        switch (hParent->balance)
            {
            case  0:
                // the insertion brought the sub-tree into balance
                return;

            case -1:
            case  1:
                // the insertion created minor imbalance; continue up the
                // tree until we either hit the head, or find a major
                // imbalance which we'll have to fix (see below)
                hChild  = hParent;
                hParent = hChild->parent;

                if (NULL == hParent)
                    {
                    return; // reached head, done
                    }

                // identify direction to child, and upate parent's balance
                hParent->balance += (hParent->left == hChild ? -1 : 1);
                continue;

            case -2:
            case  2:
                {
                // major imbalance, balance by rotation(s)
                // determine imbalance type
                bool fLeftChild = hParent->left == hChild;
                int32_t   nBal       = fLeftChild ? -1 : 1;
                if (hChild->balance == nBal)
                    {
                    // child and parent are unbalanced in the same direction;
                    // single rotation is needed to become balanced
                    hParent->balance = hChild->balance = 0; // adjust balance
                    rotate(hParent, !fLeftChild);
                    }
                else // child->balance == -nBal
                    {
                    // child and parent are unbalanced in opposing directions;
                    // double rotation is needed to become balanced
                    adjustDoubleBalance(hParent, hChild, nBal);
                    doubleRotate(hParent, !fLeftChild);
                    }
                return; // now balanced
                }
            default:
                COH_THROW (IllegalStateException::create());
            }
        }
    }

void AbstractSparseArray::balancePostRemove(
        AbstractSparseArray::Node::Handle hPruned, bool fPrunedLeft)
    {
    // walk back up the search path and rebalance tree
    while (true)
        {
        // update balance factors
        hPruned->balance += (fPrunedLeft ? 1 : -1);

        switch (hPruned->balance)
            {
            case -1:
            case  1:
                // imbalance is minor; done
                return;

            case -2:
            case  2:
                {
                // removal caused major imbalance; rebalance the opposite side
                AbstractSparseArray::Node::Handle hChild;
                int32_t nBal;
                if (fPrunedLeft)
                    {
                    hChild = hPruned->right;
                    nBal  = -1;
                    }
                else
                    {
                    hChild = hPruned->left;
                    nBal  = 1;
                    }

                if (hChild->balance == -nBal)
                    {
                    hPruned->balance = hChild->balance = 0;
                    hPruned          = rotate(hPruned, fPrunedLeft);
                    }
                else if (hChild->balance == nBal)
                    {
                    adjustDoubleBalance(hPruned, hChild, -nBal);
                    hPruned = doubleRotate(hPruned, fPrunedLeft);
                    }
                else // child->balance == 0
                    {
                    hPruned->balance = -nBal;
                    hChild ->balance =  nBal;
                    rotate(hPruned, fPrunedLeft);
                    return; // balance achieved; done
                    }

                // fall through to walk up tree
                }
            case 0:
                // walk up tree
                if (NULL == hPruned->parent)
                    {
                    return; // reached the head; done
                    }
                fPrunedLeft = hPruned->parent->left == hPruned;
                hPruned     = hPruned->parent;
                continue;

            default:
                COH_THROW (IllegalStateException::create());
            }
        }
    }


// ----- Inner class: Node --------------------------------------------------

// ----- constructor ----------------------------------------------------

AbstractSparseArray::Node::Node(int64_t lKey)
    : key(lKey), balance(0), parent(self()), left(self()), right(self())
    {
    }

AbstractSparseArray::Node::Node(const AbstractSparseArray::Node& that)
    : super(that), key(that.key), balance(that.balance), parent(self()),
      left(self()), right(self())
    {
    if (that.left != NULL)
        {
        left = cast<AbstractSparseArray::Node::Handle>(that.left->clone());
        }
    if (that.right != NULL)
        {
        right = cast<AbstractSparseArray::Node::Handle>(that.right->clone());
        }
    // left/right parent are set to this in onInit()
    }

// ----- Node interface -------------------------------------------------

void AbstractSparseArray::Node::adopt(
        AbstractSparseArray::Node::Handle hChild, bool fLeft)
    {
    if (fLeft)
        {
        left  = hChild;
        }
    else
        {
        right = hChild;
        }

    if (hChild != NULL)
        {
        hChild->parent = this;
        }
    }

bool AbstractSparseArray::Node::isLeaf() const
    {
    return NULL == left || NULL == right;
    }

bool AbstractSparseArray::Node::isLinked() const
    {
    return parent != NULL || left != NULL || right != NULL;
    }

void AbstractSparseArray::Node::dissolve()
    {
    if (left != NULL)
        {
        left->dissolve();
        }

    parent = NULL;

    if (right != NULL)
        {
        right->dissolve();
        }
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> AbstractSparseArray::Node::toString() const
    {
    String::View vs;
    if (left == NULL)
        {
        vs = COH_TO_STRING(key);
        }
    else
        {
        vs = COH_TO_STRING(left << ',' << key);
        }

    if (right != NULL)
        {
        vs = COH_TO_STRING(vs << ',' << right);
        }

    return vs;
    }

void AbstractSparseArray::Node::onInit()
    {
    super::onInit();

    // sets left/right parent to this after copy constructor is called
    if (left != NULL)
        {
        left->parent = this;
        }

    if (right != NULL)
        {
        right->parent = this;
        }
    }

COH_CLOSE_NAMESPACE2
