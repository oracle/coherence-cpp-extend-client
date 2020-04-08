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

import java.io.IOException;


public class Part
        implements PortableObject
    {
    // ----- constructors ---------------------------------------------------

    public Part()
        {
        }

    public Part(int partId, String name)
        {
        this.m_partId = partId;
        this.m_name   = name;
        }


    // ----- PortableObject interface ---------------------------------------

    public void readExternal(PofReader reader)
            throws IOException
        {
        m_partId = reader.readInt(0);
        m_name    = reader.readString(1);
        }

    public void writeExternal(PofWriter writer)
            throws IOException
        {
        writer.writeInt(0, this.m_partId);
        writer.writeString(1, this.m_name);
        }


    // ----- Object methods -------------------------------------------------

    /**
    * {@inheritDoc}
    */
    public boolean equals(Object oThat)
        {
        if (this == oThat)
            {
            return true;
            }
        if (oThat == null)
            {
            return false;
            }

        Part that = (Part) oThat;
        return m_partId == that.m_partId &&
               m_name.equals(that.m_name);
        }


    /**
    * {@inheritDoc}
    */
    public int hashCode()
        {
        return m_partId + m_name.hashCode();
        }


    // ----- accessors ------------------------------------------------------

    public int getpartId()
        {
        return m_partId;
        }

    public void setpartId(int partId)
        {
        this.m_partId = partId;
        }
		
	public String getName()
        {
        return m_name;
        }

    public void setName(String name)
        {
        this.m_name = name;
        }

    // ----- data members ---------------------------------------------------

	private int    m_partId;
    
	private String m_name;	
    }