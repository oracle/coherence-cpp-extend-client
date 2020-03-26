/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OR_FILTER_HPP
#define COH_OR_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/AnyFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which returns the logical "and" of two other filters.
*
* @author djl  2008.03.23
*/
class COH_EXPORT OrFilter
    : public class_spec<OrFilter,
        extends<AnyFilter> >
    {
    friend class factory<OrFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        OrFilter();

        /**
        * Construct an OrFilter. The result is defined as:
        *   filterLeft || filterRight
        *
        * @param vFilterLeft   the "left" filter
        * @param vFilterRight  the "right" filter
        */
        OrFilter(Filter::View vFilterLeft, Filter::View vFilterRight);

    private:
        /**
        * Blocked copy constructor.
        */
        OrFilter(const OrFilter&);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_OR_FILTER_HPP
