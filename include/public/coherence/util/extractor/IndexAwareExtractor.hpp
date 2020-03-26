/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INDEX_AWARE_EXTRACTOR_HPP
#define COH_INDEX_AWARE_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* IndexAwareExtractor is an extension to the ValueExtractor interface
* that supports the creation and destruction of an MapIndex index.
* Instances of this interface are intended to be used with the QueryMap's
* addIndex and removeIndex API to support the creation of custom indexes.
*
* @author tb/lh  2010.08.31
* @since Coherence 3.6
*/
class COH_EXPORT IndexAwareExtractor
    : public interface_spec<IndexAwareExtractor,
        implements<ValueExtractor> >
    {
    public:
        /**
        * Create an index and associate it with the corresponding extractor.
        * Important: it is a responsibility of this method's implementations
        * to place the necessary &lt;ValueExtractor, MapIndex&gt; entry into the
        * given map of indexes.
        *
        * @param fOrdered     true iff the contents of the indexed information
        *                     should be ordered; false otherwise
        * @param vComparator  the Comparator object which imposes an ordering
        *                     of entries in the index contents; or <tt>NULL</tt>
        *                     if the entries' values natural ordering should be
        *                     used
        * @param hMapIndex    a Map of ValueExtractor to MapIndex to be updated with
        *                     the created index
        *
        * @return the created index; NULL if the index has not been created
        */
        virtual MapIndex::Handle createIndex(bool fOrdered,
                Comparator::View vComparator, Map::Handle hMapIndex) const = 0;

        /**
        * Destroy an existing index and remove it from the given map of indexes.
        *
        * @param hMapIndex  a Map of ValueExtractor to MapIndex to be updated by
        *                   removing the index being destroyed
        *
        * @return the destroyed index; NULL if the index does not exist
        */
        virtual MapIndex::Handle destroyIndex(Map::Handle hMapIndex) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_INDEX_AWARE_EXTRACTOR_HPP
