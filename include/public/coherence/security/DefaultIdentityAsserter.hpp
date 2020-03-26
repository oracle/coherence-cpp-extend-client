/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DEFAULT_IDENTITY_ASSERTER_HPP
#define COH_DEFAULT_IDENTITY_ASSERTER_HPP

#include "coherence/lang.ns"

#include "coherence/net/Service.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/security/IdentityAsserter.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::Subject;
using coherence::net::Service;


/**
* The default implementation of the IdentityAsserter interface.
* <p>
* The default implementation asserts that the token is a Subject.
*
* @author dag  2009.11.16
*/
class DefaultIdentityAsserter
    : public class_spec<DefaultIdentityAsserter,
        extends<Object>,
        implements<IdentityAsserter> >
    {
    friend class factory<DefaultIdentityAsserter>;

    // ----- IdentityAsserter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Subject::View assertIdentity(Object::View vToken,
                Service::View vService) const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Get the singleton instance of the DefaultIdentityAsserter.
        */
        static DefaultIdentityAsserter::Handle getInstance();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DEFAULT_IDENTITY_ASSERTER_HPP
