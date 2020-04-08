/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IS_NULL_FILTER_HPP
#define COH_IS_NULL_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/filter/EqualsFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which compares the result of a method invocation with null.
*
* @author djl  2008.04.07
*/
class COH_EXPORT IsNullFilter
    : public class_spec<IsNullFilter,
        extends<EqualsFilter> >
    {
    friend class factory<IsNullFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        IsNullFilter();

        /**
        * Construct a IsNullFilter for testing equality to null.
        *
        * @param vValueExtractor  the ValueExtractor to use by this filter
        */
        IsNullFilter(ValueExtractor::View vValueExtractor);

    private:
        /**
        * Blocked copy constructor.
        */
        IsNullFilter(const IsNullFilter&);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_IS_NULL_FILTER_HPP
