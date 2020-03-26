/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_NAVIGATOR_HPP
#define COH_POF_NAVIGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/reflect/PofValue.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)


/**
* The PofNavigator interface represents an algorithm for navigating a PofValue
* hierarchy in order to locate a contained PofValue for extraction, modification
* and/or removal purposes.
*
* @author as/gm  2009.04.01
* @since Coherence 3.5
*/
class COH_EXPORT PofNavigator
    : public interface_spec<PofNavigator>
    {
    // ----- PofNavigator interface -----------------------------------------

    public:
        /**
        * Locate the {@link PofValue} designated by this PofNavigator within
        * the passed PofValue.
        *
        * @param hValueOrigin  the origin from which navigation starts
        *
        * @return the resulting PofValue
        *
        * @throws PofNavigationException  if the navigation fails; for
        *         example one of the intermediate nodes in this path is a
        *         "terminal" PofValue.
        */
        virtual PofValue::Handle navigate(PofValue::Handle hValueOrigin) const = 0;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_POF_NAVIGATOR_HPP
