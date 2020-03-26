/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTACT_STABLE_ITERATOR_HPP
#define COH_ABSTACT_STABLE_ITERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Muterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An abstract Iterator implementation that is stable between the hasNext() and
* next() methods.
*
* @author mf 2008.02.28
*/
class COH_EXPORT AbstractStableIterator
    : public abstract_spec<AbstractStableIterator,
        extends<Object>,
        implements<Muterator> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractStableIterator();


    // ----- Muterator interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove();


    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next();


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * Obtain the previous object provided by the Iterator.
        *
        * @return the object previously returned from a call to #next()
        */
        virtual Object::Holder getPrevious() const;

        /**
        * Specify the next object to provide from the Iterator.
        *
        * @param ohNext  the next object to provide from the Iterator
        */
        virtual void setNext(Object::Holder ohNext);

        /**
        * Advance to the next object.
        *
        * This method must be implemented by the concrete sub-class by calling
        * #setNext() if there is a next object.
        */
        virtual void advance() = 0;

        /**
        * Remove the specified item.
        *
        * This is an optional operation. If the Iterator supports element
        * removal, then it should implement this method, which is delegated to by
        * the #remove() method.
        *
        * @param ohPrev  the previously iterated object that should be removed
        *
        * @throws coherence::lang::UnsupportedOperationException if removal
        *         is not supported
        */
        virtual void remove(Object::Holder ohPrev);


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Set to true when <tt>m_oNext</tt> is the next object to return
        * from the iterator. If there is no next object, or if the next object
        * is not determined yet, then this will be false. Set up by
        * #setNext() and reset by #next().
        */
        bool m_fNextReady;

        /**
        * The next object to return from this iterator.  Set up by
        * #setNext() and reset by #next().
        */
        MemberHolder<Object> m_ohNext;

        /**
        * The object that can be deleted, if any. Set up by #next.
        */
        MemberHolder<Object> m_ohPrev;

        /**
        * Set to true when an element has been returned but not yet
        * removed. Set up by #next() and reset by #remove().
        */
        bool m_fCanDelete;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTACT_STABLE_ITERATOR_HPP
