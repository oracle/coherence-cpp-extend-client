/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DEFAULT_IDENTITY_TRANSFORMER_HPP
#define COH_DEFAULT_IDENTITY_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/net/Service.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/security/IdentityTransformer.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::Subject;
using coherence::net::Service;


/**
* The default implementation of the IdentityTransformer interface, which
* simply returns the Subject that is passed to it.
*
* @author dag 2009.12.21
*/
class DefaultIdentityTransformer
    : public class_spec<DefaultIdentityTransformer,
        extends<Object>,
        implements<IdentityTransformer> >
    {
    friend class factory<DefaultIdentityTransformer>;

    // ----- IdentityTransformer interface ----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View transformIdentity(Subject::View vSubject,
                Service::View vService) const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Get the singleton instance of the DefaultIdentityTransformer.
        */
        static DefaultIdentityTransformer::Handle getInstance();
   };

COH_CLOSE_NAMESPACE2

#endif // COH_DEFAULT_IDENTITY_TRANSFORMER_HPP
