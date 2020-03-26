/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_COLLECTION_HPP
#define COH_ABSTRACT_COLLECTION_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* This class provides a skeletal implementation of the <tt>Collection</tt>
* interface, to minimize the effort required to implement this interface.
*
* To implement an unmodifiable collection, the programmer needs only to
* extend this class and provide implementations for the <tt>size</tt> and
* <tt>iterator</tt> methods.
*
* To implement a modifiable collection, the programmer must additionally
* override this class's <tt>add</tt>, <tt>remove</tt>, <tt>removeAll</tt>,
* <tt>retainAll</tt> and <tt>clear</tt> methods
* (which otherwise throw an <tt>UnsupportedOperationException</tt>).
*
* The programmer should generally provide a void (no argument) and
* <tt>Collection</tt> constructor, as per the recommendation in the
* <tt>Collection</tt> interface specification.
*
* @see Collection
*/
class COH_EXPORT AbstractCollection
    : public abstract_spec<AbstractCollection,
        extends<Object>,
        implements<Collection> >
    {
    // ----- Collection interface --------------------------------------------

    public:
        // Query Operations

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const = 0;

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hao = NULL) const;

        /**
        * {@inheritDoc}
        *
        * This implementation iterates over the specified collection,
        * checking each element returned by the iterator in turn to see if
        * it's contained in this collection. If all elements are so
        * contained <tt>true</tt> is returned, otherwise <tt>false</tt>.
        */
        virtual bool containsAll(Collection::View vc) const;

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException unless #add() is
        * overridden (assuming the specified collection is non-empty).
        */
        virtual bool addAll(Collection::View vc);

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException unless #add() is
        * overridden (assuming the specified collection is non-empty).
        */
        virtual bool remove(Object::View v);

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException} unless #remove() is
        * overridden (assuming the specified collection is non-empty).
        */
        virtual bool removeAll(Collection::View vc);

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException unless #remove() is
        * overridden (assuming there are items to be removed by the
        * operation).
        */
        virtual bool retainAll(Collection::View vc);

        /**
        * {@inheritDoc}
        *
        * This implementation will throw a
        * coherence::lang::UnsupportedOperationException.
        */
        virtual void clear();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_COLLECTION_HPP
