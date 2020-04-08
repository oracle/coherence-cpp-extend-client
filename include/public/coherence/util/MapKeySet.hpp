/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_KEYSET_HPP
#define COH_MAP_KEYSET_HPP

#include "coherence/lang.ns"

#include "coherence/util/ConverterCollections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A wrapper class that provides the Set interface for the key set of a map.
* It's returned by the default implementation of the AbstractMap::keySet()
* method. MapKeySet is based on the ConverterCollections functionality which
* will convert a map's entrySet into a KeySet
*
* Note that because this ConverterSet is based on the Map's entrySet, all
* write operations on the set must be converted out to a method on the Map,
* not the keyset.
*
* @author nsa 2008.01.31
*/
class COH_EXPORT MapKeySet
    : public class_spec<MapKeySet,
        extends<ConverterCollections::ConverterSet> >
    {
    friend class factory<MapKeySet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a KeySet which delegates to the specified Set or Map
        * depending on the operation.
        *
        * @param hMap  the specfied Map.
        */
        MapKeySet(Map::Handle hMapDelegate);

        /**
        * Create a WrapperCollections#AbstractWrapperSet which
        * delegates to the specified Set.
        *
        * @param vMap  the specified Map.
        */
        MapKeySet(Map::View vMapDelegate);


    // ----- Set interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
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


    // ----- accessors methods ----------------------------------------------

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


    // ----- inner class: KeyConverter ------------------------------------

    public:
        /**
        * Convert a Map::Entry into a Map::Entry::Key
        */
        class KeyConverter
            : public class_spec<KeyConverter,
                extends<Object>,
                implements<Converter> >
            {
            friend class factory<KeyConverter>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new KeyConverter
                */
                KeyConverter();

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
                static KeyConverter::View getInstance();
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying map
        */
        FinalHolder<Map> f_thMapDelegate;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_KEYSET_HPP
