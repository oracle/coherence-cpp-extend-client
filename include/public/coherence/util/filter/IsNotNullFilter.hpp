/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IS_NOT_NULL_FILTER_HPP
#define COH_IS_NOT_NULL_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/filter/NotEqualsFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which tests the result of a method invocation for inequality to null.
*
* @author djl  2008.04.07
*/
class COH_EXPORT IsNotNullFilter
    : public class_spec<IsNotNullFilter,
        extends<NotEqualsFilter> >
    {
    friend class factory<IsNotNullFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        IsNotNullFilter();

        /**
        * Construct a IsNotNullFilter for testing inequality to null.
        *
        * @param vValueExtractor the ValueExtractor to use by this filter
        */
        IsNotNullFilter(ValueExtractor::View vValueExtractor);
		
    private:
        /**
        * Blocked copy constructor.
        */
        IsNotNullFilter(const IsNotNullFilter&);
   };

COH_CLOSE_NAMESPACE3

#endif // COH_IS_NOT_NULL_FILTER_HPP
