/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_KEY_ASSOCIATION_HPP
#define COH_KEY_ASSOCIATION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,net,cache)


/**
* A KeyAssociation represents a key object that has a natural association
* with another key object.
*
* The key object and the associated key may refer to entries in the same or
* different caches.
*
* For example, the information provided by a key that implements
* <b>KeyAssociation</b> may be used to place the key into the same partition
* as its associated key.
*
* See KeyAssociatedFilter for an example of a distributed query that takes
* advantage of a custom KeyAssociation implementation to dramatically optimize
* its performance.
*
* @author gm  2009.03.18
*/
class COH_EXPORT KeyAssociation
    : public interface_spec<KeyAssociation>
    {
    // ----- KeyAssociation interface ---------------------------------------

    public:
        /**
        * Determine the key object to which this key object is associated.
		* The key object returned by this method is often referred to as a
		* <i>host key</i>.
        *
        * @return the host key that for this key object, or <tt>NULL</tt> if
		*  	      this key has no association.
        */
        virtual Object::View getAssociatedKey() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_KEY_ASSOCIATION_HPP
