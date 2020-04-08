/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IDENTITY_TRANSFORMER_HPP
#define COH_IDENTITY_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/net/Service.hpp"
#include "coherence/security/auth/Subject.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::Subject;
using coherence::net::Service;


/**
* IdentityTransformer transforms a Subject to a token that asserts identity.
*
* @author dag  2009.12.04
*
* @since Coherence 3.6
*/
class COH_EXPORT IdentityTransformer
    : public interface_spec<IdentityTransformer>
    {
    // ----- IdentityTransformer interface ----------------------------------

    public:
        /**
        * Transform a Subject to a token that asserts an identity.
        *
        * @param vSubject  the Subject representing a user.
        * @param vService  the Service requesting an identity token
        *
        * @return the token that asserts identity.
        *
        * @throws SecurityException if the identity transformation fails.
        *
        * @since Coherence 3.7 added vService param which intentionally breaks
        *        compatibility with Coherence 3.6
        */
        virtual Object::View transformIdentity(Subject::View vSubject,
                Service::View vService) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_IDENTITY_TRANSFORMER_HPP
