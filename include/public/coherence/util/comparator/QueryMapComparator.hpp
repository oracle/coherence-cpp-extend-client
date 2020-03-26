/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUERY_MAP_COMPARATOR_HPP
#define COH_QUERY_MAP_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/QueryMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)


/**
* This interface is used by Comparator implementations that can use value
* extraction optimization exposed by the
* QueryMap::Entry interface.
*
* @author djl  2008.03.03
*/
class COH_EXPORT QueryMapComparator
    : public interface_spec<QueryMapComparator,
        implements<Comparator> >
    {
    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * Compare two entries based on the rules specified by Comparator.
        *
        * If possible, use the coherence::util::QueryMap.Entry::extract()
        * method to optimize the value extraction process.
        *
        * This method is expected to be implemented by Comparator wrappers,
        * such as ChainedComparator and InverseComparator, which simply pass
        * on this invocation to the wrapped Comparator objects if they too
        * implement this interface, or to invoke their default compare method
        * passing the actual objects (not the extracted values) obtained from
        * the extractor using the passed entries.
        *
        * This interface is also expected to be implemented by ValueExtractor
        * implementations that implement the Comparator interface. It is expected
        * that in most cases, the Comparator wrappers will eventually terminate
        * at (i.e. delegate to) ValueExtractors that also implement this
        * interface.
        *
        * @param vEntry1  the first entry to compare values from; read-only
        * @param vEntry2  the second entry to compare values from; read-only
        *
        * @return a negative integer, zero, or a positive integer as the first
        *         entry denotes a value that is is less than, equal to, or
        *         greater than the value denoted by the second entry
        *
        * @throws ClassCastException if the arguments' types prevent them from
        *            being compared by this Comparator.
        * @throws IllegalArgumentException if the extractor cannot handle
        *         the passed objects for any other reason; an implementor should
        *         include a descriptive message
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                QueryMap::Entry::View vEntry2) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_QUERY_MAP_COMPARATOR_HPP
