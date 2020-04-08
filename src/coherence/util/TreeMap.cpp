/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/TreeMap.hpp"

#include "coherence/util/AbstractSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(TreeMap)

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
        implements<Iterator> >
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
        * @internal
        */
        Crawler(TreeMap::Holder hTreeMap,
                RelativePosition fromDir,
                Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive)
            : f_hTreeMap(self(), hTreeMap),
              m_hCurrent(self(), hTreeMap->getHeadNode(), true),
              m_hNext(self(), (TreeMap::Node::Handle) NULL, true),
              m_fromDir(fromDir),
              f_vFromKey(self(), vFromKey),
              m_fFromInclusive(fromInclusive),
              f_vToKey(self(), vToKey),
              m_fToInclusive(toInclusive)
            {
            if (f_vFromKey != NULL)
                {
                // advance current until current key is >= (or > if
                // fromInclusive is false) vFromKey.
                while (hasNext())
                    {
                    TreeMap::Node::Handle hNode = nextNode(true);
                    if ((m_fFromInclusive &&
                            hTreeMap->compare(hNode->f_vKey, f_vFromKey) >= 0) ||
                            (!m_fFromInclusive &&
                            hTreeMap->compare(hNode->f_vKey, f_vFromKey) > 0))
                        {
                        break;
                        }
                    nextNode(false);
                    }
                }
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            if (m_hNext != NULL)
                {
                return true;
                }

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
                        if (m_hCurrent->m_hLeft != NULL)
                            {
                            m_hCurrent = m_hCurrent->m_hLeft;
                            break;
                            }
                        m_fromDir = LEFT;
                        // no break;

                    case LEFT:
                        // the current node is the next element to return
                        if (f_vToKey != NULL &&
                                ((!m_fToInclusive &&
                                f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) >= 0) ||
                                (m_fToInclusive &&
                                f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) > 0)))
                            {
                            return false;
                            }
                        return true;

                    case SITTING:
                        // try to crawl down to the right
                        if (m_hCurrent->m_hRight != NULL)
                            {
                            m_fromDir  = ABOVE;
                            m_hCurrent = m_hCurrent->m_hRight;
                            break;
                            }
                        // no break;

                    case RIGHT:
                        // try to crawl up
                        if (m_hCurrent->m_hParent != NULL)
                            {
                            m_fromDir = m_hCurrent ==
                                    m_hCurrent->m_hParent->m_hLeft ?
                                    LEFT : RIGHT;
                            m_hCurrent = cast
                                    <TreeMap::Node::Handle>
                                    (m_hCurrent->m_hParent);
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

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            TreeMap::Node::Handle hNextNode = nextNode(false);

            if (hNextNode == NULL)
                {
                COH_THROW (NoSuchElementException::create());
                }
            return hNextNode;
            }

    // ----- Object interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            switch (m_fromDir)
                {
                case ABOVE:
                    return COH_TO_STRING("crawled into " << m_hCurrent->f_vKey);
                case LEFT:
                    return COH_TO_STRING("returned to " << m_hCurrent->f_vKey <<
                            " from the left child");
                case SITTING:
                    return COH_TO_STRING("sitting in " << m_hCurrent->f_vKey);
                case RIGHT:
                    return COH_TO_STRING("returned to " << m_hCurrent->f_vKey <<
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
        * @return the next Node in the iteration; NULL if the iterator has
        *         no more elements
        */
        virtual TreeMap::Node::Handle nextNode(bool fPeek) const
            {
            if (m_hNext != NULL)
                {
                m_hCurrent = m_hNext;

                if (!fPeek)
                    {
                    m_fromDir = SITTING;
                    m_hNext = NULL;
                    }
                return m_hCurrent;
                }

            if (NULL == m_hCurrent || (f_vToKey != NULL &&
                    ((!m_fToInclusive &&
                    f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) >= 0) ||
                    (m_fToInclusive &&
                    f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) > 0))))
                {
                return NULL;
                }

            // save the original state so that we can restore it
            TreeMap::Node::Handle hOrigCurrent = m_hCurrent;
            RelativePosition      origFromDir  = m_fromDir;

            while (true)
                {
                switch (m_fromDir)
                    {
                    case ABOVE:
                        // try to crawl down to the left
                        if (m_hCurrent->m_hLeft != NULL)
                            {
                            m_hCurrent = m_hCurrent->m_hLeft;
                            break;
                            }
                        // no break;

                    case LEFT:
                        {
                        // the current node is the next element to return
                        TreeMap::Node::Handle hReturnVal = m_hCurrent;
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

                        if (f_vToKey != NULL &&
                                ((!m_fToInclusive &&
                                f_hTreeMap->compare(hReturnVal->f_vKey, f_vToKey) >= 0) ||
                                (m_fToInclusive &&
                                f_hTreeMap->compare(hReturnVal->f_vKey, f_vToKey) > 0)))
                            {
                            return NULL;
                            }
                        return hReturnVal;
                        }
                    case SITTING:
                        // try to crawl down to the right
                        if (m_hCurrent->m_hRight != NULL)
                            {
                            m_fromDir = ABOVE;
                            m_hCurrent = m_hCurrent->m_hRight;
                            break;
                            }
                        // no break;

                    case RIGHT:
                        // try to crawl up
                        if (m_hCurrent->m_hParent != NULL)
                            {
                            m_fromDir = m_hCurrent ==
                                    m_hCurrent->m_hParent->m_hLeft ?
                                    LEFT : RIGHT;
                            m_hCurrent = cast<TreeMap::Node::Handle>
                                    (m_hCurrent->m_hParent);
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
                        return NULL;

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

        TreeMap::Node::Handle currentNode() const
            {
            TreeMap::Node::Handle hNode = m_hCurrent;
            if (NULL == hNode || m_fromDir != SITTING)
                {
                COH_THROW (IllegalStateException::create());
                }
            return hNode;
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * Tree map
        */
        FinalHolder<TreeMap> f_hTreeMap;

        /**
        * Current Node
        */
        mutable MemberHandle<TreeMap::Node> m_hCurrent;

        /**
        * Next Node
        */
        mutable MemberHandle<TreeMap::Node> m_hNext;

        /**
        * Traversal direction
        */
        mutable RelativePosition m_fromDir;

        /**
        * The lower range of this iterator
        */
        FinalView<Object> f_vFromKey;

        /**
        * Whether the lower endpoint is to be included in the returned view
        */
        const bool m_fFromInclusive;

        /**
        * The upper range of this iterator
        */
        FinalView<Object> f_vToKey;

        /**
        * Whether the upper endpoint is to be included in the returned view
        */
        const bool m_fToInclusive;
    };


// ----- local class: MutatingCrawler -----------------------------------

/**
* MutatingCrawler is mutating tree node iterator, that is it is capable of
* changing the tree map it iterates.
*/
class MutatingCrawler
    : public class_spec<MutatingCrawler,
        extends<Crawler>,
        implements<Muterator> >
    {
    friend class factory<MutatingCrawler>;

    // ----- constructors -------------------------------------------

    protected:
        MutatingCrawler(TreeMap::Holder hTreeMap,
                RelativePosition fromDir,
                Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive)
            : super(hTreeMap, fromDir, vFromKey, fromInclusive, vToKey, toInclusive)
            {
            }

    // ----- Muterator interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            if (NULL != m_hNext || NULL == m_hCurrent ||
                    (NULL != f_vToKey &&
                    ((!m_fToInclusive && f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) >= 0) ||
                    (m_fToInclusive && f_hTreeMap->compare(m_hCurrent->f_vKey, f_vToKey) > 0))))
                {
                COH_THROW (NoSuchElementException::create());
                }

            Object::View hRemoveKey = m_hCurrent->f_vKey;
            m_hNext = nextNode(true);
            cast<TreeMap::Handle>(f_hTreeMap)->remove(hRemoveKey);
            }
    };


// ----- local class: EntrySet ------------------------------------------

/**
* A set of entries backed by a tree map.
*/
class COH_EXPORT EntrySet
    : public class_spec<EntrySet,
        extends<AbstractSet> >
    {
    friend class factory<EntrySet>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * @internal
        */
        EntrySet(TreeMap::Holder hTreeMap)
                : super(),
                  f_hTreeMap(self(), hTreeMap),
                  f_vFromKey(self(), NULL),
                  m_fFromInclusive(true),
                  f_vToKey(self(), NULL),
                  m_fToInclusive(false)
            {
            }

        /**
        * @internal
        */
        EntrySet(TreeMap::Holder hTreeMap,
                Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive)
                : super(),
                  f_hTreeMap(self(), hTreeMap),
                  f_vFromKey(self(), vFromKey),
                  m_fFromInclusive(fromInclusive),
                  f_vToKey(self(), vToKey),
                  m_fToInclusive(toInclusive)
            {
            }

    // ----- Set interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh)
        {
        TreeMap::Handle hMap = getDelegate();
        COH_SYNCHRONIZED (hMap)
            {
            Map::Entry::Holder hEntry = cast<Map::Entry::Holder>(oh);
            return hMap->put(hEntry->getKey(), hEntry->getValue()) != NULL;
            }
        }

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            TreeMap::View vMap = getDelegate();
            COH_SYNCHRONIZED (vMap)
                {
                if (f_vFromKey == NULL && f_vToKey == NULL)
                    {
                    return vMap->size();
                    }
                size32_t nSize = 0;
                for (Iterator::Handle hIter = iterator(); hIter->hasNext();
                        ++nSize)
                    {
                    hIter->next();
                    }
                return nSize;
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v)
            {
            TreeMap::Handle hMap = getDelegate();
            COH_SYNCHRONIZED (hMap)
                {
                return hMap->remove((cast<Map::Entry::View>(v))->getKey())
                        != NULL;
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            TreeMap::Handle hMap = getDelegate();
            COH_SYNCHRONIZED (hMap)
                {
                if (f_vFromKey == NULL && f_vToKey == NULL)
                    {
                    hMap->clear();
                    }
                else
                    {
                    for (Muterator::Handle hIter = iterator();
                            hIter->hasNext();)
                        {
                        hIter->next();
                        hIter->remove();
                        }
                    }
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return Crawler::create(getDelegate(), Crawler::ABOVE,
                    f_vFromKey, m_fFromInclusive, f_vToKey, m_fToInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return MutatingCrawler::create(getDelegate(), Crawler::ABOVE,
                    f_vFromKey, m_fFromInclusive, f_vToKey, m_fToInclusive);
            }

    TreeMap::View getDelegate() const
        {
        return f_hTreeMap;
        }

    TreeMap::Handle getDelegate()
        {
        return cast<TreeMap::Handle>(f_hTreeMap);
        }

    // ----- data members -------------------------------------------

    protected:
        /**
        * The tree map.
        */
        FinalHolder<TreeMap> f_hTreeMap;

        /**
        * The lower range of this entry set
        */
        FinalView<Object> f_vFromKey;

        /**
        * lower endpoint is to be included in the returned view
        */
        const bool m_fFromInclusive;

        /**
        * The upper range of this entry set
        */
        FinalView<Object> f_vToKey;

        /**
        * Whether the upper endpoint is to be included in the returned view
        */
        const bool m_fToInclusive;
    };

// ----- local class: SubMap --------------------------------------------

/**
* A sub map of a tree map.
*/
class COH_EXPORT SubMap
    : public class_spec<SubMap,
        extends<AbstractMap>,
        implements<NavigableMap> >
    {
    friend class factory<SubMap>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * @internal
        */
        SubMap(TreeMap::Holder hTreeMap,
                Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive)
                : super(),
                  f_hTreeMap(self(), hTreeMap),
                  f_vFromKey(self(), vFromKey),
                  m_fFromInclusive(fromInclusive),
                  f_vToKey(self(), vToKey),
                  m_fToInclusive(toInclusive)
            {
            }

    // ----- Map interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue)
            {
            TreeMap::Handle hMap = getDelegate();
            COH_SYNCHRONIZED (hMap)
                {
                if (!inRange(vKey))
                    {
                    COH_THROW (IllegalArgumentException::create(
                            "key out of range"));
                    }
                return cast<TreeMap::Handle>(hMap)->put(vKey, ohValue);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey)
            {
            TreeMap::Handle hMap = getDelegate();
            COH_SYNCHRONIZED (hMap)
                {
                if (!inRange(vKey))
                    {
                    return NULL;
                    }
                return hMap->remove(vKey);
                }
            }
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const
            {
            TreeMap::View vMap = getDelegate();
            if (!inRange(vKey))
                {
                return NULL;
                }
            return vMap->get(vKey);
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey)
            {
            TreeMap::Handle hMap = getDelegate();
            if (!inRange(vKey))
                {
                return NULL;
                }
            return hMap->get(vKey);
            }

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            entrySet()->clear();
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const
            {
            return EntrySet::create(getDelegate(), f_vFromKey, m_fFromInclusive,
                    f_vToKey, m_fToInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet()
            {
            return EntrySet::create(getDelegate(), f_vFromKey, m_fFromInclusive,
                    f_vToKey, m_fToInclusive);
            }

    // ----- Sorted Map interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Comparator::View comparator() const
            {
            return getDelegate()->comparator();
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::View firstKey() const
            {
            TreeMap::View vMap = getDelegate();
            if (NULL == f_vFromKey)
               {
               return vMap->firstKey();
               }
            Object::View vKey = m_fFromInclusive ? vMap->ceilingKey(f_vFromKey) :
                     vMap->higherKey(f_vFromKey);
            if (vKey == NULL || tooHigh(vKey))
                {
                COH_THROW (NoSuchElementException::create());
                }
            return vKey;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::View lastKey() const
            {
            TreeMap::View vMap = getDelegate();
            if (NULL == f_vToKey)
               {
               return vMap->lastKey();
               }
            Object::View vKey = m_fToInclusive ? vMap->floorKey(f_vToKey) :
                     vMap->lowerKey(f_vToKey);
            if (vKey == NULL || tooLow(vKey))
                {
                COH_THROW (NoSuchElementException::create());
                }
            return vKey;
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle headMap(Object::View vToKey)
            {
            return SubMap::create(getDelegate(), f_vFromKey, true, vToKey, false);
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View headMap(Object::View vToKey) const
            {
            return SubMap::create(getDelegate(), f_vFromKey, true, vToKey, false);
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle subMap(Object::View vFromKey,
                Object::View vToKey)
            {
            return SubMap::create(getDelegate(), vFromKey, true, vToKey, false);
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View subMap(Object::View vFromKey,
                Object::View vToKey) const
            {
            return SubMap::create(getDelegate(), vFromKey, true, vToKey, false);
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle tailMap(Object::View vFromKey)
            {
            return SubMap::create(getDelegate(), vFromKey, true, f_vToKey, false);
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View tailMap(Object::View vFromKey) const
            {
            return SubMap::create(getDelegate(), vFromKey, true, f_vToKey, false);
            }

    // ----- NavigableMap interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View ceilingKey(Object::View vKey) const
            {
            TreeMap::View vMap = getDelegate();
            if (tooLow(vKey))
                {
                return vMap->ceilingKey(f_vFromKey);
                }
            Object::View vCeilingKey = m_fFromInclusive
                    ? vMap->ceilingKey(vKey)
                    : vMap->higherKey(vKey);
            return (vCeilingKey == NULL || tooHigh(vCeilingKey)) ? NULL : vCeilingKey;
            }

        /**
         * {@inheritDoc}
         */
        virtual Object::View floorKey(Object::View vKey) const
            {
            TreeMap::View vMap = getDelegate();
            if (tooHigh(vKey))
                {
                return vMap->floorKey(f_vToKey);
                }
            Object::View vFloorKey = m_fToInclusive
                    ? vMap->floorKey(vKey)
                    : vMap->lowerKey(vKey);
            return (vFloorKey == NULL || tooLow(vFloorKey)) ? NULL : vFloorKey;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::View higherKey(Object::View vKey) const
            {
            TreeMap::View vMap = getDelegate();
            if (tooLow(vKey))
                {
                return vMap->higherKey(f_vFromKey);
                }
            Object::View vHigherKey = vMap->higherKey(vKey);
            return (vHigherKey == NULL || tooHigh(vHigherKey)) ? NULL : vHigherKey;
            }

        /**
         * {@inheritDoc}
         */
        virtual Object::View lowerKey(Object::View vKey) const
            {
            TreeMap::View vMap = getDelegate();
            if (tooHigh(vKey))
                {
                return vMap->lowerKey(f_vToKey);
                }
            Object::View vLowerKey = vMap->lowerKey(vKey);
            return (vLowerKey == NULL || tooLow(vLowerKey)) ? NULL : vLowerKey;
            }

        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::Holder pollFirstEntry()
            {
            TreeMap::Handle hMap = getDelegate();
            TreeMap::Node::Handle hNode = hMap->find(firstKey());
            hMap->removeNode(hNode);
            return hNode;
            }

        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::Holder pollLastEntry()
            {
            TreeMap::Handle hMap = getDelegate();
            TreeMap::Node::Handle hNode = hMap->find(lastKey());
            hMap->removeNode(hNode);
            return hNode;
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle headMap(Object::View vToKey,
                bool toInclusive)
            {
            return SubMap::create(getDelegate(), f_vFromKey, m_fFromInclusive,
                    vToKey, toInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View headMap(Object::View vToKey,
                bool toInclusive) const
            {
            return SubMap::create(getDelegate(), f_vFromKey, m_fFromInclusive,
                    vToKey, toInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle subMap(Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive)
            {
            return SubMap::create(getDelegate(), vFromKey, fromInclusive,
                    vToKey, toInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View subMap(Object::View vFromKey,
                bool fromInclusive,
                Object::View vToKey,
                bool toInclusive) const
            {
            return SubMap::create(getDelegate(), vFromKey, fromInclusive,
                    vToKey, toInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::Handle tailMap(Object::View vFromKey,
                bool fromInclusive)
            {
            return SubMap::create(getDelegate(), vFromKey, fromInclusive,
                    f_vToKey, m_fToInclusive);
            }

        /**
        * {@inheritDoc}
        */
        virtual NavigableMap::View tailMap(Object::View vFromKey,
                bool fromInclusive) const
            {
            return SubMap::create(getDelegate(), vFromKey, fromInclusive,
                    f_vToKey, m_fToInclusive);
            }

    // ----- helper methods -----------------------------------------

        TreeMap::View getDelegate() const
            {
            return f_hTreeMap;
            }

        TreeMap::Handle getDelegate()
            {
            return cast<TreeMap::Handle>(f_hTreeMap);
            }

        bool tooLow(Object::View vKey) const
            {
            if (f_vFromKey != NULL)
                {
                int32_t c = getDelegate()->compare(vKey, f_vFromKey);
                if (c < 0 || (c == 0 && !m_fFromInclusive))
                    {
                    return true;
                    }
                }
            return false;
            }

        bool tooHigh(Object::View vKey) const
            {
            if (f_vToKey != NULL)
                {
                int32_t c = getDelegate()->compare(vKey, f_vToKey);
                if (c > 0 || (c == 0 && !m_fToInclusive))
                    {
                    return true;
                    }
                }
            return false;
            }

        bool inRange(Object::View vKey) const
            {
            return !tooLow(vKey) && !tooHigh(vKey);
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * The tree map.
        */
        FinalHolder<TreeMap> f_hTreeMap;

        /**
        * The lower range of this submap
        */
        FinalView<Object> f_vFromKey;

        /**
        * Whether the lower endpoint is to be included in the returned view
        */
        const bool m_fFromInclusive;

        /**
        * The upper range of this submap
        */
        FinalView<Object> f_vToKey;

        /**
        * Whether the upper endpoint is to be included in the returned view
        */
        const bool m_fToInclusive;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructor --------------------------------------------------------

TreeMap::TreeMap()
    : m_nSize(0), m_hHead(self(), (TreeMap::Node::Handle) NULL, true),
      f_vComparator(self())
    {
    }

TreeMap::TreeMap(Comparator::View vComparator)
    : m_nSize(0), m_hHead(self(), (TreeMap::Node::Handle) NULL, true),
      f_vComparator(self(), vComparator)
    {
    }

TreeMap::TreeMap(const TreeMap& that)
    : super(), m_nSize(that.m_nSize),
      m_hHead(self(), (TreeMap::Node::Handle) NULL, true),
      f_vComparator(self(), that.f_vComparator)
    {
    if (that.m_hHead != NULL)
        {
        m_hHead = cast<TreeMap::Node::Handle>
                (that.m_hHead->clone());
        }
    }

TreeMap::~TreeMap()
    {
    clear(); // unlink or it will be leaked
    }

// ----- Map interface ------------------------------------------------------

size32_t TreeMap::size() const
    {
    return m_nSize;
    }

bool TreeMap::isEmpty() const
    {
    return size() == 0;
    }

bool TreeMap::containsKey(Object::View vKey) const
    {
    return find(vKey) != NULL;
    }

Object::Holder TreeMap::get(Object::View vKey) const
    {
    Node::Handle hNode = find(vKey);
    return (NULL == hNode ? (Object::Holder) NULL : hNode->getValue());
    }

Object::Holder TreeMap::put(Object::View vKey, Object::Holder ohValue)
    {
    Node::Handle hNode = findInsertionPoint(vKey);
    if (hNode != NULL &&
            compare(hNode->f_vKey, vKey) == 0)
        {
        return hNode->setValue(ohValue);
        }
    balancedInsertion(hNode, instantiateNode(vKey, ohValue));
    return NULL;
    }

Object::Holder TreeMap::remove(Object::View vKey)
    {
    Node::Handle hNode = find(vKey);
    if (NULL == hNode)
        {
        return NULL;
        }
    removeNode(hNode);
    return hNode->getValue();
    }

void TreeMap::clear()
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

Set::View TreeMap::entrySet() const
    {
    return EntrySet::create(View(this));
    }

Set::Handle TreeMap::entrySet()
    {
    return EntrySet::create(Handle(this));
    }


// ----- Sorted Map interface -----------------------------------------------

Comparator::View TreeMap::comparator() const
    {
    return f_vComparator;
    }

Object::View TreeMap::firstKey() const
    {
    TreeMap::Node::View vNodeCur = m_hHead;
    if (NULL == vNodeCur)
        {
        COH_THROW (NoSuchElementException::create());
        }

    TreeMap::Node::View vNodeNext = vNodeCur->m_hLeft;
    while (vNodeNext != NULL)
        {
        vNodeCur  = vNodeNext;
        vNodeNext = vNodeCur->m_hLeft;
        }

    return vNodeCur->f_vKey;
    }

Object::View TreeMap::lastKey() const
    {
    TreeMap::Node::View vNodeCur = m_hHead;
    if (NULL == vNodeCur)
        {
        COH_THROW (NoSuchElementException::create());
        }

    TreeMap::Node::View vNodeNext = vNodeCur->m_hRight;
    while (vNodeNext != NULL)
        {
        vNodeCur  = vNodeNext;
        vNodeNext = vNodeCur->m_hRight;
        }

    return vNodeCur->f_vKey;
    }

SortedMap::Handle TreeMap::headMap(Object::View vToKey)
    {
    return SubMap::create(this, (Object::View) NULL, true, vToKey, false);
    }

SortedMap::View TreeMap::headMap(Object::View vToKey) const
    {
    return SubMap::create(this, (Object::View) NULL, true, vToKey, false);
    }

SortedMap::Handle TreeMap::subMap(Object::View vFromKey,
        Object::View vToKey)
    {
    return SubMap::create(this, vFromKey, true, vToKey, false);
    }

SortedMap::View TreeMap::subMap(Object::View vFromKey,
        Object::View vToKey) const
    {
    return SubMap::create(this, vFromKey, true, vToKey, false);
    }

SortedMap::Handle TreeMap::tailMap(Object::View vFromKey)
    {
    return SubMap::create(this, vFromKey, true, (Object::View) NULL, false);
    }

SortedMap::View TreeMap::tailMap(Object::View vFromKey) const
    {
    return SubMap::create(this, vFromKey, true, (Object::View) NULL, false);
    }


// ----- Navigable Map interface --------------------------------------------

Object::View TreeMap::ceilingKey(Object::View vKey) const
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int32_t cmp = compare(vKey, hCurrent->f_vKey);
        if (cmp < 0)
            {
            if (hCurrent->m_hLeft != NULL)
                {
                hCurrent = hCurrent->m_hLeft;
                }
            else
                {
                return hCurrent->f_vKey;
                }
            }
        else if (cmp > 0)
            {
            if (hCurrent->m_hRight != NULL)
                {
                hCurrent = hCurrent->m_hRight;
                }
            else
                {
                TreeMap::Node::Handle hParent = hCurrent->m_hParent;
                TreeMap::Node::Handle ch = hCurrent;
                while (hParent != NULL && ch == hParent->m_hRight)
                    {
                    ch = hParent;
                    hParent = hParent->m_hParent;
                    }
                return hParent == NULL ? (Object::View) NULL : hParent->f_vKey;
                }
            }
        else
            {
            return hCurrent->f_vKey;
            }
        }
    return NULL;
    }

Object::View TreeMap::floorKey(Object::View vKey) const
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int32_t cmp = compare(vKey, hCurrent->f_vKey);
        if (cmp > 0)
            {
            if (hCurrent->m_hRight != NULL)
                {
                hCurrent = hCurrent->m_hRight;
                }
            else
                {
                return hCurrent->f_vKey;
                }
            }
        else if (cmp < 0)
            {
            if (hCurrent->m_hLeft != NULL)
                {
                hCurrent = hCurrent->m_hLeft;
                }
            else
                {
                TreeMap::Node::Handle hParent = hCurrent->m_hParent;
                TreeMap::Node::Handle ch = hCurrent;
                while (hParent != NULL && ch == hParent->m_hLeft)
                    {
                    ch = hParent;
                    hParent = hParent->m_hParent;
                    }
                return hParent == NULL ? (Object::View) NULL : hParent->f_vKey;
                }
            }
        else
            {
            return hCurrent->f_vKey;
            }
        }
    return NULL;
    }

Object::View TreeMap::higherKey(Object::View vKey) const
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int32_t cmp = compare(vKey, hCurrent->f_vKey);
        if (cmp < 0)
            {
            if (hCurrent->m_hLeft != NULL)
                {
                hCurrent = hCurrent->m_hLeft;
                }
            else
                {
                return hCurrent->f_vKey;
                }
            }
        else
            {
            if (hCurrent->m_hRight != NULL)
                {
                hCurrent = hCurrent->m_hRight;
                }
            else
                {
                TreeMap::Node::Handle hParent = hCurrent->m_hParent;
                TreeMap::Node::Handle ch = hCurrent;
                while (hParent != NULL && ch == hParent->m_hRight)
                    {
                    ch = hParent;
                    hParent = hParent->m_hParent;
                    }
                return hParent == NULL ? (Object::View) NULL : hParent->f_vKey;
                }
            }
        }
    return NULL;
    }

Object::View TreeMap::lowerKey(Object::View vKey) const
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        int32_t cmp = compare(vKey, hCurrent->f_vKey);
        if (cmp > 0)
            {
            if (hCurrent->m_hRight != NULL)
                {
                hCurrent = hCurrent->m_hRight;
                }
            else
                {
                return hCurrent->f_vKey;
                }
            }
        else
            {
            if (hCurrent->m_hLeft != NULL)
                {
                hCurrent = hCurrent->m_hLeft;
                }
            else
                {
                TreeMap::Node::Handle hParent = hCurrent->m_hParent;
                TreeMap::Node::Handle ch = hCurrent;
                while (hParent != NULL && ch == hParent->m_hLeft)
                    {
                    ch = hParent;
                    hParent = hParent->m_hParent;
                    }
                return hParent == NULL ? (Object::View) NULL : hParent->f_vKey;
                }
            }
        }
    return NULL;
    }

Map::Entry::Holder TreeMap::pollFirstEntry()
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    if (hCurrent != NULL)
        {
        while (hCurrent->m_hLeft != NULL)
            {
            hCurrent = hCurrent->m_hLeft;
            }
        }
    removeNode(hCurrent);
    return hCurrent;
    }

Map::Entry::Holder TreeMap::pollLastEntry()
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    if (hCurrent != NULL)
        {
        while (hCurrent->m_hRight != NULL)
            {
            hCurrent = hCurrent->m_hRight;
            }
        }
    removeNode(hCurrent);
    return hCurrent;
    }

NavigableMap::Handle TreeMap::headMap(Object::View vToKey, bool toInclusive)
    {
    return SubMap::create(this, (Object::View) NULL, true, vToKey, toInclusive);
    }

NavigableMap::View TreeMap::headMap(Object::View vToKey, bool toInclusive) const
    {
    return SubMap::create(this, (Object::View) NULL, true, vToKey, toInclusive);
    }

NavigableMap::Handle TreeMap::subMap(Object::View vFromKey, bool fromInclusive,
        Object::View vToKey, bool toInclusive)
    {
    return SubMap::create(this, vFromKey, fromInclusive, vToKey, toInclusive);
    }

NavigableMap::View TreeMap::subMap(Object::View vFromKey,bool fromInclusive,
        Object::View vToKey, bool toInclusive) const
    {
    return SubMap::create(this, vFromKey, fromInclusive, vToKey, toInclusive);
    }

NavigableMap::Handle TreeMap::tailMap(Object::View vFromKey, bool fromInclusive)
    {
    return SubMap::create(this, vFromKey, fromInclusive, (Object::View) NULL, false);
    }

NavigableMap::View TreeMap::tailMap(Object::View vFromKey, bool fromInclusive) const
    {
    return SubMap::create(this, vFromKey, fromInclusive, (Object::View) NULL, false);
    }


// ----- Helper methods -----------------------------------------------------

int32_t TreeMap::compare(Object::View v1, Object::View v2) const
    {
    if (NULL == f_vComparator)
        {
        return cast<Comparable::View>(v1)->compareTo(v2);
        }
    return f_vComparator->compare(v1, v2);
    }

bool TreeMap::isHead(TreeMap::Node::View vNode) const
    {
    return Object::equals(m_hHead, vNode);
    }

TreeMap::Node::Handle TreeMap::find(
        Object::View ovKey) const
    {
    TreeMap::Node::Handle hCurrent = m_hHead;
    while (hCurrent != NULL)
        {
        Object::View ovCurrent = hCurrent->f_vKey;
        if (compare(ovKey, ovCurrent) < 0)
            {
            hCurrent = hCurrent->m_hLeft;
            }
        else if (compare(ovKey, ovCurrent) > 0)
            {
            hCurrent = hCurrent->m_hRight;
            }
        else // lIndex == lCurrent
            {
            return hCurrent;
            }
        }

    return NULL;
    }

TreeMap::Node::Handle TreeMap::getHeadNode() const
    {
    return m_hHead;
    }

void TreeMap::removeNode(TreeMap::Node::Handle hNode)
    {
    if (NULL == hNode->m_hLeft || NULL == hNode->m_hRight)
        {
        // at most one child node is NULL, perform simple adoption;
        // node's parent replaces node with node's only child
        TreeMap::Node::Handle hChild  =
                NULL == hNode->m_hLeft ? hNode->m_hRight : hNode->m_hLeft;
        TreeMap::Node::Handle hParent = replace(hNode, hChild);

        if (hParent != NULL)
            {
            // parent's sub-tree shrunk
            balancePostRemove(hParent, compare(hNode->f_vKey, hParent->f_vKey) < 0);
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
        TreeMap::Node::Handle hHeir = hNode->m_hRight;
        while (hHeir->m_hLeft != NULL)
            {
            hHeir = hHeir->m_hLeft;
            }

        // heir will replace node; taking it's children and balance
        hHeir->nBalance = hNode->nBalance;

        // perform adoptions
        if (hHeir->m_hParent == hNode)
            {
            // heir's parent is node; heir keeps it right (only) child,
            // and adopts node's left
            hHeir->adopt(hNode->m_hLeft, true);

            // heir will take node's place but with shorter right tree
            replace(hNode, hHeir);
            balancePostRemove(hHeir, false);
            }
        else
            {
            // heir->m_hParent != node, therefore heir == parent->left
            // heir's parent adopts heir's only child, replacing heir
            hHeir->m_hParent->adopt(hHeir->m_hRight, true);

            // heir adopts node's children
            hHeir->adopt(hNode->m_hLeft, true);
            hHeir->adopt(hNode->m_hRight, false);

            // node's parent's left tree shrunk
            TreeMap::Node::Handle hPruned = hHeir->m_hParent;
            replace(hNode, hHeir);
            balancePostRemove(hPruned, true);
            }
        }

    // invalidate any Crawler which may be sitting on node
    hNode->m_hParent = hNode->m_hRight = hNode->m_hLeft = NULL;
    --m_nSize;
    }

TreeMap::Node::Handle TreeMap::replace(
        TreeMap::Node::Handle hNodeA,
        TreeMap::Node::Handle hNodeB)
    {
    TreeMap::Node::Handle hParent = hNodeA->m_hParent;

    if (NULL == hParent)
        {
        m_hHead = hNodeB;
        }
    else if (hParent->m_hLeft == hNodeA)
        {
        hParent->m_hLeft = hNodeB;
        }
    else
        {
        hParent->m_hRight = hNodeB;
        }

    if (hNodeB != NULL)
        {
        hNodeB->m_hParent = hParent;
        }

    return hParent;
    }

TreeMap::Node::Handle TreeMap::rotate(
        TreeMap::Node::Handle hNode, bool fLeft)
    {
    TreeMap::Node::Handle hParent = hNode->m_hParent;
    TreeMap::Node::Handle hChild  =
            fLeft ? hNode->m_hRight : hNode->m_hLeft;

    replace(hChild, fLeft ? hChild->m_hLeft : hChild->m_hRight); // push grand up
    hChild->adopt(hNode, fLeft);                           // push node down

    hNode->m_hParent = hParent;       // revert node's parent for replace
    replace(hNode, hChild);        // push child up
    return hNode->m_hParent = hChild; // restore and return node's new parent
    }

TreeMap::Node::Handle TreeMap::doubleRotate(
        TreeMap::Node::Handle hNode, bool fLeft)
    {
    rotate(fLeft ? hNode->m_hRight : hNode->m_hLeft, !fLeft); // rotate child
    return rotate(hNode, fLeft);                        // rotate node
    }

void TreeMap::adjustDoubleBalance(
        TreeMap::Node::Handle hNode,
        TreeMap::Node::Handle hChild, int32_t nBal)
    {
    TreeMap::Node::Handle hGrand =
            hChild == hNode->m_hLeft ? hChild->m_hRight : hChild->m_hLeft;

    if (hGrand->nBalance == 0)
        {
        hNode->nBalance = hChild->nBalance = 0;
        }
    else if (hGrand->nBalance == nBal)
        {
        hNode->nBalance  = -nBal;
        hChild->nBalance = 0;
        }
    else
        {
        hNode->nBalance  = 0;
        hChild->nBalance = nBal;
        }
    hGrand->nBalance = 0;
    }

TreeMap::Node::Handle TreeMap::findInsertionPoint(
        Object::View ovIndex) const
    {
    TreeMap::Node::Handle hNode = m_hHead;
    if (NULL == hNode)
        {
        return NULL;
        }

    while (true)
        {
        Object::View ovCurr = hNode->f_vKey;
        if (compare(ovIndex, ovCurr) > 0)
            {
            if (NULL == hNode->m_hRight)
                {
                return hNode;
                }
            else
                {
                hNode = hNode->m_hRight;
                }
            }
        else if (compare(ovIndex, ovCurr) < 0)
            {
            if (NULL == hNode->m_hLeft)
                {
                return hNode;
                }
            else
                {
                hNode = hNode->m_hLeft;
                }
            }
        else
            {
            return hNode;
            }
        }
    }

void TreeMap::balancedInsertion(
        TreeMap::Node::Handle hParent,
        TreeMap::Node::Handle hChild)
    {
    if (NULL == hParent)
        {
        m_hHead = hChild;
        m_nSize = 1;
        return; // done
        }
    else if (compare(hChild->f_vKey, hParent->f_vKey) < 0)
        {
        hParent->adopt(hChild, true);
        hParent->nBalance -= 1;
        }
    else
        {
        hParent->adopt(hChild, false);
        hParent->nBalance += 1;
        }
    ++m_nSize;

    // walk from the new child node up towards the head, terminating
    // once the sub-trees have been sufficiently balanced
    while (true)
        {
        switch (hParent->nBalance)
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
                hParent = hChild->m_hParent;

                if (NULL == hParent)
                    {
                    return; // reached head, done
                    }

                // identify direction to child, and update parent's balance
                hParent->nBalance += (hParent->m_hLeft == hChild ? -1 : 1);
                continue;

            case -2:
            case  2:
                {
                // major imbalance, balance by rotation(s)
                // determine imbalance type
                bool fLeftChild = hParent->m_hLeft == hChild;
                int32_t   nBal       = fLeftChild ? -1 : 1;
                if (hChild->nBalance == nBal)
                    {
                    // child and parent are unbalanced in the same direction;
                    // single rotation is needed to become balanced
                    hParent->nBalance = hChild->nBalance = 0; // adjust balance
                    rotate(hParent, !fLeftChild);
                    }
                else // child->nBalance == -nBal
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

void TreeMap::balancePostRemove(
        TreeMap::Node::Handle hPruned, bool fPrunedLeft)
    {
    // walk back up the search path and re-balance tree
    while (true)
        {
        // update balance factors
        hPruned->nBalance += (fPrunedLeft ? 1 : -1);

        switch (hPruned->nBalance)
            {
            case -1:
            case  1:
                // imbalance is minor; done
                return;

            case -2:
            case  2:
                {
                // removal caused major imbalance; re-balance the opposite side
                TreeMap::Node::Handle hChild;
                int32_t nBal;
                if (fPrunedLeft)
                    {
                    hChild = hPruned->m_hRight;
                    nBal   = -1;
                    }
                else
                    {
                    hChild = hPruned->m_hLeft;
                    nBal   = 1;
                    }

                if (hChild->nBalance == -nBal)
                    {
                    hPruned->nBalance = hChild->nBalance = 0;
                    hPruned           = rotate(hPruned, fPrunedLeft);
                    }
                else if (hChild->nBalance == nBal)
                    {
                    adjustDoubleBalance(hPruned, hChild, -nBal);
                    hPruned = doubleRotate(hPruned, fPrunedLeft);
                    }
                else // child->nBalance == 0
                    {
                    hPruned->nBalance = -nBal;
                    hChild ->nBalance =  nBal;
                    rotate(hPruned, fPrunedLeft);
                    return; // balance achieved; done
                    }

                // fall through to walk up tree
                }
            case 0:
                // walk up tree
                if (NULL == hPruned->m_hParent)
                    {
                    return; // reached the head; done
                    }
                fPrunedLeft = hPruned->m_hParent->m_hLeft == hPruned;
                hPruned     = hPruned->m_hParent;
                continue;

            default:
                COH_THROW (IllegalStateException::create());
            }
        }
    }

TreeMap::Node::Handle TreeMap::instantiateNode(Object::View ovKey,
        Object::Holder ohValue)
    {
    TreeMap::Node::Handle hNode = Node::create(ovKey);
    hNode->setValue(ohValue);
    return hNode;
    }

Muterator::Handle TreeMap::iterator()
    {
    return MutatingCrawler::create(this, Crawler::ABOVE, (Object::View) NULL,
            true, (Object::View) NULL, false);
    }

Iterator::Handle TreeMap::iterator() const
    {
    return Crawler::create(this, Crawler::ABOVE, (Object::View) NULL,
            true, (Object::View) NULL, false);
    }


// ----- Inner class: Node --------------------------------------------------

// ----- constructor ----------------------------------------------------

TreeMap::Node::Node(Object::View ovKey)
    : f_vKey(self(), ovKey), m_ohValue(self()), nBalance(0), m_hParent(self()),
      m_hLeft(self()), m_hRight(self())
    {
    }

TreeMap::Node::Node(const TreeMap::Node& that)
    : super(that), f_vKey(self(), that.f_vKey), m_ohValue(self(),
      that.m_ohValue), nBalance(that.nBalance), m_hParent(self()),
      m_hLeft(self()), m_hRight(self())
    {
    if (that.m_hLeft != NULL)
        {
        m_hLeft = cast<TreeMap::Node::Handle>(that.m_hLeft->clone());
        }
    if (that.m_hRight != NULL)
        {
        m_hRight = cast<TreeMap::Node::Handle>(that.m_hRight->clone());
        }
    // m_hLeft/right parent are set to this in onInit()
    }

// ----- Node interface -------------------------------------------------

void TreeMap::Node::adopt(TreeMap::Node::Handle hChild, bool fLeft)
    {
    if (fLeft)
        {
        m_hLeft  = hChild;
        }
    else
        {
        m_hRight = hChild;
        }

    if (hChild != NULL)
        {
        hChild->m_hParent = this;
        }
    }

Object::Holder TreeMap::Node::getValue() const
    {
    return m_ohValue;
    }

Object::Holder TreeMap::Node::getValue()
    {
    return cast<Object::Holder>(m_ohValue);
    }

Object::Holder TreeMap::Node::setValue(Object::Holder ohValue)
    {
    Object::Holder ohOldValue = m_ohValue;
    m_ohValue = ohValue;
    return ohOldValue;
    }

Object::View TreeMap::Node::getKey() const
    {
    return f_vKey;
    }

bool TreeMap::Node::isLeaf() const
    {
    return NULL == m_hLeft || NULL == m_hRight;
    }

void TreeMap::Node::dissolve()
    {
    if (m_hLeft != NULL)
        {
        m_hLeft->dissolve();
        }

    m_hParent = NULL;

    if (m_hRight != NULL)
        {
        m_hRight->dissolve();
        }
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> TreeMap::Node::toString() const
    {
    String::View vs = "";
    if (m_hLeft != NULL)
        {
        vs = COH_TO_STRING(vs <<  m_hLeft << ',');
        }

    vs = COH_TO_STRING(vs <<  f_vKey);

    if (m_hRight != NULL)
        {
        vs = COH_TO_STRING(vs <<  ',' << m_hRight);
        }

    return vs;
    }

void TreeMap::Node::onInit()
    {
    super::onInit();

    // sets left/right parent to this after copy constructor is called
    if (m_hLeft != NULL)
        {
        m_hLeft->m_hParent = this;
        }

    if (m_hRight != NULL)
        {
        m_hRight->m_hParent = this;
        }
    }

COH_CLOSE_NAMESPACE2
