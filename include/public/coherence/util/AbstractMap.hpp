/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_MAP_HPP
#define COH_ABSTRACT_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* This class provides a skeletal implementation of the <tt>Map</tt>
* interface, to minimize the effort required to implement this interface.
*
* To implement an unmodifiable map, the programmer needs only to extend this
* class and provide implementations for the entrySet() and
* iterator() methods.  Method <tt>entrySet</tt> returns a set-view of
* the map's mappings. Typically, the returned set will, in turn, be
* implemented atop <tt>AbstractDeepSet</tt>. Method <tt>iterator</tt> returns
* an entry set iterator that points to the mapping for specified key.
*
* To implement a modifiable map, the programmer must additionally override
* this class's <tt>put</tt>, <tt>remove</tt> and <tt>clear</tt> methods
* (which otherwise throw an
* {@link coherence::lang::UnsupportedOperationException
* UnsupportedOperationException}.
*
* The programmer should generally provide a void (no argument) and map
* constructor, as per the recommendation in the <tt>Map</tt> interface
* specification.
*
* @see Map
*
* @author nsa,mf 2008.01.30
*/
class COH_EXPORT AbstractMap
    : public abstract_spec<AbstractMap,
        extends<Describable>,
        implements<Map> >
    {
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
        virtual bool containsValue(Object::View vValue) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void putAll(Map::View vMap);

        /**
        * {@inheritDoc}
        */
        virtual Set::View keySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle keySet();

        /**
        * {@inheritDoc}
        */
        virtual Collection::View values() const;

        /**
        * {@inheritDoc}
        */
        virtual Collection::Handle values();


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

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_MAP_HPP
