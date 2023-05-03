/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;


import com.tangosol.io.pof.PortableObject;
import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;

import com.tangosol.util.Versionable;

import java.io.IOException;


/**
* Simple Versionable implementation.
*
* @author tb  2008.06.09
*/
public class VersionedObject
        implements Versionable, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
    public VersionedObject()
        {
        super();
        }


    // ----- Versionable interface ------------------------------------

    public Comparable getVersionIndicator()
        {
        return m_LVersion;
        }

    public void incrementVersion()
        {
        m_LVersion = m_LVersion + 1;
        }


    // ----- PortableObject interface ---------------------------------------

    /**
    * {@inheritDoc}
    */
    public void readExternal(PofReader in)
            throws IOException
        {
        m_LVersion = in.readLong(0);
        }

    /**
    * {@inheritDoc}
    */
    public void writeExternal(PofWriter out)
            throws IOException
        {
        out.writeLong(0, m_LVersion);
        }


    // ----- data members ---------------------------------------------------

    private Long m_LVersion;
    }
