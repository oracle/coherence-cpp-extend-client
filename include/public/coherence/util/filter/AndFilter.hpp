/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_AND_FILTER_HPP
#define COH_AND_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/AllFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::util::Filter;


/**
* Filter which returns the logical "and" of two other filters.
*
* @author djl  2008.03.23
*/
class COH_EXPORT AndFilter
    : public class_spec<AndFilter,
        extends<AllFilter> >
    {
    friend class factory<AndFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        AndFilter();

        /**
        * Construct an "and" filter. The result is defined as:
        *   vFilterLeft &amp;&amp; vFilterRight
        *
        * @param vFilterLeft   the "left" filter
        * @param vFilterRight  the "right" filter
        */
        AndFilter(Filter::View vFilterLeft, Filter::View vFilterRight);

    private:
        /**
        * Blocked copy constructor.
        */
        AndFilter(const AndFilter&);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_AND_FILTER_HPP
