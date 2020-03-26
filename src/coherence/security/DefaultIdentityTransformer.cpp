/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/security/DefaultIdentityTransformer.hpp"


COH_OPEN_NAMESPACE2(coherence,security)


// ----- IdentityTransformer interface --------------------------------------

Object::View DefaultIdentityTransformer::transformIdentity(
        Subject::View vSubject, Service::View /*vService*/) const
    {
    return vSubject;
    }


// ---- constants -----------------------------------------------------------

DefaultIdentityTransformer::Handle DefaultIdentityTransformer::getInstance()
    {
    static FinalHandle<DefaultIdentityTransformer> hDIT(System::common(),
            DefaultIdentityTransformer::create());
    return hDIT;
    }
COH_STATIC_INIT(DefaultIdentityTransformer::getInstance());

COH_CLOSE_NAMESPACE2
