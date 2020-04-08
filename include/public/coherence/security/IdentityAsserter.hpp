/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IDENTITY_ASSERTER_HPP
#define COH_IDENTITY_ASSERTER_HPP

#include "coherence/lang.ns"

#include "coherence/net/Service.hpp"
#include "coherence/security/auth/Subject.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::Subject;
using coherence::net::Service;


/**
* IdentityAsserter validates a token in order to establish a user's identity.
* The token is an identity assertion, a statement that asserts an identity.
* <p>
* A token is opaque to Coherence. It could be a standard type such as a SAML
* Assertion or a proprietary type.
*
* @author dag 2009.10.30
*
* @since Coherence 3.6
*/
class COH_EXPORT IdentityAsserter
    : public interface_spec<IdentityAsserter>
    {
    // ----- IdentityAsserter interface -------------------------------------

    public:
        /**
        * Asserts an identity based on a token-based identity assertion.
        *
        * @param oToken    the token that asserts identity.
        * @param vService  the Service asserting the identity token
        *
        * @return a Subject representing the identity.
        *
        * @throws SecurityException if the identity assertion fails.
        *
        * @since Coherence 3.7 added vService param which intentionally breaks
        *        compatibility with Coherence 3.6
        */
        virtual Subject::View assertIdentity(Object::View vToken,
                Service::View vService) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_IDENTITY_ASSERTER_HPP
