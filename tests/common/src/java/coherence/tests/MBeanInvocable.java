/*
 * Copyright (c) 2000, 2026, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;


import com.tangosol.io.pof.PortableObject;
import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;

import com.tangosol.net.CacheFactory;
import com.tangosol.net.Cluster;
import com.tangosol.net.Invocable;
import com.tangosol.net.InvocationService;

import com.tangosol.net.management.MBeanServerProxy;
import com.tangosol.net.management.Registry;

import com.tangosol.util.Base;

import java.io.IOException;

import java.util.Iterator;
import java.util.Set;

import javax.management.MBeanServer;
import javax.management.ObjectInstance;
import javax.management.ObjectName;


/**
* Invocable implementation that queries the Member attribute of the
* ConnectionMBean and returns the string value.
*
* @author lh  2010.01.24
*/
public class MBeanInvocable
        implements Invocable, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
    public MBeanInvocable()
        {
        }


    // ----- Invocable interface --------------------------------------------

    /**
    * {@inheritDoc}
    */
    public void init(InvocationService service)
        {
        assert service.getInfo().getServiceType()
                .equals(InvocationService.TYPE_REMOTE);
        m_service = service;
        }

    /**
    * {@inheritDoc}
    */
    public void run()
        {
        if (m_service != null)
            {
            Cluster cluster   = CacheFactory.getCluster();
            Registry registry = cluster.getManagement();
            assert registry != null;

            try
                {
                String sValue = getConnectionMember(registry);
                if (sValue == null)
                    {
                    Base.log("DBG: MBeanInvocable.run(); test failed to find Coherence Connection MBean");
                    }
                else
                    {
                    setValue(sValue);
                    }
                }
            catch (Exception e)
                {
                throw Base.ensureRuntimeException(e);
                }
            }
        }

    /**
    * {@inheritDoc}
    */
    public Object getResult()
        {
        return m_sValue;
        }

    /**
    * Return the Member attribute from the local ConnectionMBean.
    *
    * @param registry  the cluster management registry
    *
    * @return the Member attribute value, or null if no ConnectionMBean is found
    */
    protected String getConnectionMember(Registry registry)
        {
        MBeanServerProxy proxy    = registry.getMBeanServerProxy();
        String           sName    = registry.ensureGlobalName(Registry.CONNECTION_TYPE);
        String           sDomain  = registry.getDomainName();
        String           sPattern = (sName.indexOf(':') < 0
                ? (sDomain == null || sDomain.length() == 0 ? "Coherence" : sDomain) + ':' + sName
                : sName) + ",*";

        for (int i = 0; i < 40; ++i)
            {
            String sValue = proxy.execute(server -> getConnectionMember(server, sPattern));
            if (sValue != null)
                {
                return sValue;
                }

            Base.sleep(250L);
            }

        Base.log("DBG: MBeanInvocable.run(); failed to find Coherence Connection MBean, pattern: " + sPattern);
        return null;
        }

    /**
    * Return the Member attribute from the local ConnectionMBean.
    *
    * @param server    the MBeanServer
    * @param sPattern  the ConnectionMBean name pattern
    *
    * @return the Member attribute value, or null if no ConnectionMBean is found
    */
    protected static String getConnectionMember(MBeanServer server, String sPattern)
        {
        try
            {
            Set<ObjectInstance> setBeans = server.queryMBeans(new ObjectName(sPattern), null);
            for (Iterator<ObjectInstance> iter = setBeans.iterator(); iter.hasNext();)
                {
                ObjectName objectName = iter.next().getObjectName();
                try
                    {
                    Object oValue = server.getAttribute(objectName, "Member");
                    if (oValue != null)
                        {
                        Base.log("DBG: MBeanInvocable.run(); found Coherence Connection MBean " + objectName);
                        return oValue.toString();
                        }
                    }
                catch (Exception e)
                    {
                    Base.log("DBG: MBeanInvocable.run(); ignored unavailable Coherence Connection MBean "
                            + objectName + ": " + e);
                    }
                }
            }
        catch (Exception e)
            {
            throw Base.ensureRuntimeException(e);
            }

        return null;
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

    // ----- accessors ------------------------------------------------------

    /**
    * Set the string value.
    *
    * @param sValue  the value of the attribute
    */
    public void setValue(String sValue)
        {
        m_sValue = sValue;
        }


    // ----- data members ---------------------------------------------------

    /**
    * The string value of the attribute.
    */
    private transient String m_sValue;

    /**
    * The InvocationService that is executing this Invocable.
    */
    private transient InvocationService m_service;
    }
