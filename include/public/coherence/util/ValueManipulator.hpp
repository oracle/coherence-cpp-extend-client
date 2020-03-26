/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VALUE_MANIPULATOR_HPP
#define COH_VALUE_MANIPULATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* ValueManipulator represents a composition of ValueExtractor and
* ValueManipulator implementations.
*
* @author tb  2008.05.06
*/
class COH_EXPORT ValueManipulator
    : public interface_spec<ValueManipulator>
    {
    // ----- ValueManipulator interface -------------------------------------

    public:
        /**
        * Retrieve the underlying ValueExtractor reference.
        *
        * @return the ValueExtractor
        */
        virtual ValueExtractor::View getExtractor() const = 0;

        /**
        * Retrieve the underlying ValueUpdater reference.
        *
        * @return the ValueUpdater
        */
        virtual ValueUpdater::View getUpdater() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VALUE_MANIPULATOR_HPP
