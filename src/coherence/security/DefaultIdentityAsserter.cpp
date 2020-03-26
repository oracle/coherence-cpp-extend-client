/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/security/DefaultIdentityAsserter.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,security)


// ----- IdentityAsserter interface -----------------------------------------

Subject::View DefaultIdentityAsserter::assertIdentity(Object::View vToken, 
		Service::View /*vService*/) const
    {
    // support old behavior where a null token is passed if no Subject is
    // in the client context
    if (vToken == NULL)
        {
        return NULL;
        }
    if (instanceof<Subject::View>(vToken))
        {
        return cast<Subject::View>(vToken);
        }
    COH_LOG("expected Subject but found: " << vToken, 1);
    COH_THROW(SecurityException::create("Invalid identity token"));
    }


// ---- constants -----------------------------------------------------------

DefaultIdentityAsserter::Handle DefaultIdentityAsserter::getInstance()
    {
    static FinalHandle<DefaultIdentityAsserter> hDIA(System::common(),
            DefaultIdentityAsserter::create());
    return hDIA;
    }
COH_STATIC_INIT(DefaultIdentityAsserter::getInstance());

COH_CLOSE_NAMESPACE2
