/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_EVEN_FILTER_HPP
#define COH_TEST_EVEN_FILTER_HPP

#include "coherence/lang.ns"
#include "coherence/util/Filter.hpp"

using coherence::util::Filter;

class TestEvenFilter
    : public class_spec<TestEvenFilter,
        extends<Object>,
        implements<Filter> >
    {
    friend class factory<TestEvenFilter>;

    public:
        virtual bool evaluate(Object::View v) const
            {
            Integer32::View vValue = cast<Integer32::View>(v);
            return vValue->getInt32Value() % 2 == 0;
            }
    };

#endif // #define COH_TEST_EVEN_FILTER_HPP
