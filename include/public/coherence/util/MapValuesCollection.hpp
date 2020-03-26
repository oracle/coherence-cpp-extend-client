/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_VALUES_COLLECTION_HPP
#define COH_MAP_VALUES_COLLECTION_HPP

#include "coherence/lang.ns"

#include "coherence/util/ConverterCollections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A wrapper class that provides the Collection interface for the value
* collection of a map. It's returned by the default implementation of the
* AbstractMap::values() method. MapValuesCollection is based on the
* ConverterCollections functionality which will convert a map's entrySet
* into a KeySet
*
* Note that because this ConverterSet is based on the Map's entrySet,
* all write operations on the set must be converted out to a method on
* the Map, not the keyset.
*
* @author nsa 2008.01.30
*/
class COH_EXPORT MapValuesCollection
    : public class_spec<MapValuesCollection,
        extends<ConverterCollections::ConverterCollection> >
    {
    friend class factory<MapValuesCollection>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a WrapperCollections#AbstractWrapperCollection which
        * delegates to the specified Collection.
        *
        * @param hMapDelegate  the underlying map
        */
        MapValuesCollection(Map::Handle hMapDelegate);

        /**
        * Create a WrapperCollections#AbstractWrapperCollection which
        * delegates to the specified Collection.
        *
        * @param vMapDelegate  the underlying map
        */
        MapValuesCollection(Map::View vMapDelegate);


    // ----- Collection interface -------------------------------------------

    public:
        /**
        * @{inheritDoc}
        */
        virtual bool remove(Object::View v);

        /**
        * @{inheritDoc}
        */
        virtual bool removeAll(Collection::View vc);

        /**
        * @{inheritDoc}
        */
        virtual bool retainAll(Collection::View vc);

        /**
        * @{inheritDoc}
        */
        virtual bool contains(Object::View v) const;

        /**
        * @{inheritDoc}
        */
        virtual bool containsAll(Collection::View vc) const;


    // ----- accessors ------------------------------------------------------

    protected:
        /**
        * Return the delegate Map instance.
        *
        * @return the delegate Map instance
        */
        virtual Map::Handle getMapDelegate();

        /**
        * Return the delegate Map instance.
        *
        * @return the delegate Map instance
        */
        virtual Map::View getMapDelegate() const;


    // ----- inner class: ValueConverter ------------------------------------

    public:
        /**
        * Convert a Map::Entry into a Map::Entry::Key
        */
        class ValueConverter
            : public class_spec<ValueConverter,
                extends<Object>,
                implements<Converter> >
            {
            friend class factory<ValueConverter>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new ValueConverter
                */
                ValueConverter();

            // ----- Converter interface --------------------------------

            public:
                /**
                * Convert an {@link coherence::lang::Object Object} referenced
                * by the supplied holder.
                *
                * @param oh the holder to the Object to convert
                *
                * @return the holder to the converted Object
                */
                virtual Object::Holder convert(Object::Holder oh) const;

                /**
                * Return a singleton instance of the ValueConverter.
                */
                static ValueConverter::View getInstance();
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Map to which this KeySet is tied
        */
        FinalHolder<Map> f_thMapDelegate;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_VALUES_COLLECTION_HPP
