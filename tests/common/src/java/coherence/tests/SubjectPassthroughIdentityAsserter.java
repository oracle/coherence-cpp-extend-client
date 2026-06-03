/*
 * Copyright (c) 2000, 2026, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;

import com.tangosol.net.Service;

import com.tangosol.net.security.IdentityAsserter;

import javax.security.auth.Subject;

/**
 * Test identity asserter that accepts a Subject identity token from an Extend
 * client and returns it unchanged for server-side request execution.
 *
 * @author phf  2026.06.03
 * @since 14.1.2.0.7
 */
public class SubjectPassthroughIdentityAsserter
        implements IdentityAsserter
    {
    @Override
    public Subject assertIdentity(Object oToken, Service service)
            throws SecurityException
        {
        if (oToken == null || oToken instanceof Subject)
            {
            return (Subject) oToken;
            }

        throw new SecurityException("identity token is unsupported type: "
                + oToken.getClass().getName());
        }
    }
