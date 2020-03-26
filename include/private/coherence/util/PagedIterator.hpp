/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PAGED_ITERATOR_HPP
#define COH_PAGED_ITERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* PagedIterator is an Iterator implementation based on a concept of a <i>page
* Advancer</i> - a pluggable component that knows how to supply a next page
* of objects to iterate through. As common to iterators, this implementation
* is not thread safe.
*
* @author tb  2008.08.11
*/
class COH_EXPORT PagedIterator
    : public class_spec<PagedIterator,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<PagedIterator>;

    // ----- inner class: Advancer ------------------------------------------

    public:
    /**
    * Advancer is a pluggable component that knows how to load a new page
    * (Collection) of objects to be used by the enclosing PagedIterator.
    */
    class COH_EXPORT Advancer
        : public interface_spec<Advancer>
        {

        // ----- Advancer interface -------------------------------------

        public:
            /**
            * Obtain a new page of objects to be used by the enclosing
            * PagedIterator.
            *
            * @return a Collection of objects or null if the Advancer is
            *         exhausted
            */
            virtual Collection::Handle nextPage() = 0;

            /**
            * Remove the specified object from the underlying collection.
            * Naturally, only an object from the very last non-empty page
            * could be removed.
            *
            * @param vCurr  currently "active" item to be removed from an
            *               underlying collection
            */
            virtual void remove(Object::View vCurr) = 0;
        };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PagedIterator instance
        *
        * @param hAdvancer  the underlying Advancer
        */
        PagedIterator(Advancer::Handle hAdvancer);

    private:
        /**
        * Blocked copy constructor.
        */
        PagedIterator(const PagedIterator&);


    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext()const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next();


    // ----- data fields --------------------------------------------------

    protected:
        /**
        * The underlying Advancer.
        */
        mutable FinalHandle<Advancer> f_hAdvancer;

        /**
        * An Iterator for the current page.
        */
        mutable MemberHandle<Iterator> m_hIterPage;

        /**
        * Currently "Active" object.
        */
        MemberHolder<Object> m_ohCurr;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PAGED_ITERATOR_HPP
