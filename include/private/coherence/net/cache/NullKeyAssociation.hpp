/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NULL_KEY_ASSOCIATION_HPP
#define COH_NULL_KEY_ASSOCIATION_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/KeyAssociation.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)


/**
* A dummy KeyAssociation implementation, required by the linker in order to
* export interface symbols.
*
* @author gm 2009.03.17
*/
class COH_EXPORT NullKeyAssociation
    : public class_spec<NullKeyAssociation,
		extends<Object>,
        implements<KeyAssociation> >
    {
    // ----- KeyAssociation interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getAssociatedKey() const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_NULL_KEY_ASSOCIATION_HPP
