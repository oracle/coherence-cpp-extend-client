/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOXING_MAP_HPP
#define COH_BOXING_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/stl/adapter_map.hpp"



COH_OPEN_NAMESPACE2(coherence,stl)


/**
* boxing_map provides an STL-like map adapter around a coherence::util::map,
* and additionally auto-boxes the keys and values to their corresponding
* non-managed types. The key and value template parameters specify the managed
* class which will be stored in the Map. Other then the definition, the access
* to the keys and values may use either the managed view type or the
* corresponding unmanaged type. This also allows for easy copying of mapped
* entries from a std::map to coherence maps and caches.
*
* @code
* boxing_map<Managed<Foo>, Managed<Bar> > mycache
*         (CacheFactory::getCache("mycache")); // declare boxing adapter
*
* Foo foo(...); // initialize non-managed object Foo
* Bar bar(...); // initialize non-managed object Bar
*
* mycache[foo] = bar;         // insert directly into cache via auto-boxing
* Bar barRead = mycache[foo]; // read from cache using auto-boxing
*
* std::map<Foo, Bar> mymap;
* mymap.insert(mycache.begin(), mycache.end()); // copy from coherence to STL
* mycache.insert(mymap.begin(), mymap.end());   // copy from STL to coherence
* @endcode
*
* @see coherence::util::TypedCollections::TypedMap for an Object based equivalent
*
* @author mf  2008.06.02
*/
template<class K, class V>
class boxing_map
        : public adapter_map<BoxHandle<const K, typename K::BoxedType, false>,
                             BoxHandle<const V, typename V::BoxedType, false> >
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Super type
        */
        typedef adapter_map<BoxHandle<const K, typename K::BoxedType, false>,
                            BoxHandle<const V, typename V::BoxedType, false> >
                super;


    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create new boxing_map from the given Coherence Map.
        *
        * @param ohMap the Map to be delegated by the boxing_map.
        */
        boxing_map(Map::Holder ohMap = NULL)
            : super(ohMap)
            {
            }

        /**
        * Create a boxing_map copy. The new boxing_map will reference the
        * same Map, as the original boxing_map.
        */
        boxing_map(const boxing_map& that)
            : super(that)
            {
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Reassign this boxing_map to reference the same Map as another
        * boxing_map.
        */
        boxing_map& operator=(const boxing_map& that)
            {
            super::operator=(that);
            return *this;
            }

        /**
        * Reassign this boxing_map to reference another Map.
        */
        boxing_map& operator=(Map::Holder ohMap)
            {
            super::operator=(ohMap);
            return *this;
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_BOXING_MAP_HPP
