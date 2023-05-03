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

import com.tangosol.net.cache.KeyAssociation;

import java.io.IOException;


public class OrderKey
        implements PortableObject, KeyAssociation
    {
    // ----- constructors ---------------------------------------------------

    public OrderKey()
        {
        }

    public OrderKey(int id)
        {
        this.m_id = id;
        }

    public Object getAssociatedKey()
        {
        return Integer.valueOf(m_id);
        }

    // ----- PortableObject interface ---------------------------------------

    public void readExternal(PofReader reader)
            throws IOException
        {
        m_id = reader.readInt(0);
        }

    public void writeExternal(PofWriter writer)
            throws IOException
        {
        writer.writeInt(0, m_id);
        }


    // ----- accessors ------------------------------------------------------

    public int getId()
        {
        return m_id;
        }

    public void setId(int id)
        {
        this.m_id = id;
        }

    // ----- data members ---------------------------------------------------

    private int m_id;
    }
