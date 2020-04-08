/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRINCIPAL_HPP
#define COH_PRINCIPAL_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,security)

/**
* A Principal represents a portion of an overall identity, such as a person,
* or group.
*
* @see Subject
*
* @author mf  2008.08.22
*/
class Principal
    : public interface_spec<Principal>
    {
    // ----- Principal interface --------------------------------------------

    public:
        /**
        * Return the name of this principal.
        *
        * @return the name of this principal
        */
        virtual String::View getName() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PRINCIPAL_HPP
