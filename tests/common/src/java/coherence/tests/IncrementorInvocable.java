/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;


import com.tangosol.io.pof.PortableObject;
import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;

import com.tangosol.net.Invocable;
import com.tangosol.net.InvocationService;

import java.io.IOException;


/**
* Example Invocable implementation that returns the result of incrementing a
* given number.
*
* @author jh  2008.06.06
*/
public class IncrementorInvocable
        implements Invocable, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
    public IncrementorInvocable()
        {
        super();
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
        m_nResult = m_n + 1;
        }

    /**
    * {@inheritDoc}
    */
    public Object getResult()
        {
        return Integer.valueOf(m_nResult);
        }


    // ----- PortableObject interface ---------------------------------------

    /**
    * {@inheritDoc}
    */
    public void readExternal(PofReader in)
            throws IOException
        {
        setNumber(in.readInt(0));
        }

    /**
    * {@inheritDoc}
    */
    public void writeExternal(PofWriter out)
            throws IOException
        {
        out.writeInt(0, getNumber());
        }


    // ----- accessors ------------------------------------------------------

    /**
    * Return the number to increment.
    *
    * @return the number to increment
    */
    public int getNumber()
        {
        return m_n;
        }

    /**
    * Configure the number to increment.
    *
    * @param n  the number to increment
    */
    public void setNumber(int n)
        {
        m_n = n;
        }


    // ----- data members ---------------------------------------------------

    /**
    * The number to increment.
    */
    protected int m_n;

    /**
    * The result of the Invocable.
    */
    protected transient int m_nResult;
    }
