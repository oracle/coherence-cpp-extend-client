/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SECURITY_HELPER_HPP
#define COH_SECURITY_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/security/auth/Subject.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::Subject;

/**
* SecurityHelper provides access to security related information.
*
* @author mf  2008.08.25
*/
class COH_EXPORT SecurityHelper
    : public abstract_spec<SecurityHelper>
    {
    // ----- SecurityHelper interface ---------------------------------------

    public:
        /**
        * Update the current subject to the supplied value, and return the
        * prior value.
        *
        * @param vSubject  the new subject
        *
        * @return the prior subject
        */
        static Subject::View updateCurrentSubject(Subject::View vSubject);

        /**
        * Return the current subject for the calling thread.
        *
        * @return the current subject for the calling thread
        */
        static Subject::View getCurrentSubject();
    };
COH_CLOSE_NAMESPACE2

#endif // COH_SECURITY_HELPER_HPP
