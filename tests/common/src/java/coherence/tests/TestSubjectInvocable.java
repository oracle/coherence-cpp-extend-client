/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
package coherence.tests;


import com.tangosol.io.pof.PortableObject;
import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;

import com.tangosol.net.Invocable;
import com.tangosol.net.InvocationService;

import java.io.IOException;
import java.security.*;
import javax.security.auth.*;


/**
* Invocable implementation that returns the current Subject.
*
* @author js  2008.08.28
*/
public class TestSubjectInvocable
        implements Invocable, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
    public TestSubjectInvocable()
        {
        }


    // ----- Invocable interface --------------------------------------------

    /**
    * {@inheritDoc}
    */
    public void init(InvocationService service)
        {
        }

    /**
    * {@inheritDoc}
    */
    public void run()
        {
        AccessControlContext ctx = AccessController.getContext();
        m_subject = Subject.getSubject(ctx);
        }

    /**
    * {@inheritDoc}
    */
    public Object getResult()
        {
        return m_subject;
        }


    // ----- PortableObject interface ---------------------------------------

    /**
    * {@inheritDoc}
    */
    public void readExternal(PofReader in)
            throws IOException
        {
        }

    /**
    * {@inheritDoc}
    */
    public void writeExternal(PofWriter out)
            throws IOException
        {
        }


    // ----- data members ---------------------------------------------------

    /**
    * The result of the Invocable.
    */
    protected transient Subject m_subject;
    }

