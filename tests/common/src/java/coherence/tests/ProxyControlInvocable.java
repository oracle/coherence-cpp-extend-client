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

import com.tangosol.net.CacheFactory;
import com.tangosol.net.AbstractInvocable;

import java.io.IOException;

/**
* Invocable implementation that controls a proxy service.
*
* @author par  2012.12.12
*/
public class ProxyControlInvocable
        extends AbstractInvocable
        implements PortableObject
    {
    // ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
    public ProxyControlInvocable()
        {
        }

    // ----- Invocable interface --------------------------------------------

    /**
    * {@inheritDoc}
    */
    public void run()
        {
        String sService = getProxyServiceName();

        if ((m_nOperation == STOP_OPERATION || m_nOperation == RESTART_OPERATION) &&
            CacheFactory.getCluster().getService(sService) != null)
            {
            // go through the "front door" to get the SafeService
            CacheFactory.getService(sService).shutdown();
            }

        if (m_nOperation == START_OPERATION || m_nOperation == RESTART_OPERATION)
            {
            CacheFactory.getService(sService).start();
            }
        }

    // ----- PortableObject interface ---------------------------------------

    /**
    * {@inheritDoc}
    */
    public void readExternal(PofReader in)
            throws IOException
        {
        setProxyServiceName(in.readString(0));
        m_nOperation = in.readInt(1);
        }

    /**
    * {@inheritDoc}
    */
    public void writeExternal(PofWriter out)
            throws IOException
        {
        out.writeString(0, getProxyServiceName());
        out.writeInt(1, m_nOperation);
        }

    // ----- accessors ------------------------------------------------------

    /**
    * Return the proxy service to stop.
    *
    * @return the proxy service name
    */
    public String getProxyServiceName()
        {
        return m_proxyServiceName;
        }

    /**
    * Configure the proxy service to stop.
    *
    * @param name  the proxy service to stop
    */
    public void setProxyServiceName(String name)
        {
        m_proxyServiceName = name;
        }

    // ----- constants ------------------------------------------------------
    
    /**
     * Start the service.
     */
    static public final int START_OPERATION = 1;

    /**
     * Stop the service.
     */
    static public final int STOP_OPERATION = 2;

    /**
     * Restart the service.
     */
    static public final int RESTART_OPERATION = 3;

    // ----- data members ---------------------------------------------------

    /**
    * The service name to stop.
    */
    protected String m_proxyServiceName;

    /**
    * Operation
    */
    protected int m_nOperation;
    }
