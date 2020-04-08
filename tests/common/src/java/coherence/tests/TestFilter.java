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

import com.tangosol.util.Base;
import com.tangosol.util.Filter;

import java.io.IOException;

import java.util.Map;
import java.util.Objects;

/**
 * Simple filter for validation of custom Filters with view-scheme configurations.
 * Wouldn't recommend its use elsewhere; it doesn't do much.
 *
 * @author rl 2019.05.14
 * @since 12.2.1.4
 */ 
public class TestFilter
        implements Filter, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    public TestFilter()
        {
        this(null, -1);
        }

    public TestFilter(String s, int count)
        {
        m_sAttr = null;
        m_cCount = -1;
        }

    // ----- Filter interface -----------------------------------------------

    public boolean evaluate(Object o)
        {
        return true;
        }

    // ----- accessors ------------------------------------------------------

    public String getAttr()
        {
        return m_sAttr;
        }

    public void setAttr(String sAttr)
        {
        m_sAttr = sAttr;
        }

    public int getCount()
        {
        return m_cCount;
        }

    public void setCount(int cCount)
        {
        m_cCount = cCount;
        }

    // ----- PortableObject interface ---------------------------------------

    @Override
    public void readExternal(PofReader reader) throws IOException
        {
        m_sAttr  = reader.readString(0);
        m_cCount = reader.readInt(1);
        }

    @Override
    public void writeExternal(PofWriter writer) throws IOException
        {
        writer.writeString(0, m_sAttr);
        writer.writeInt(1, m_cCount);
        }

    // ----- object methods -------------------------------------------------

    public boolean equals(Object other)
        {
        if (other instanceof TestFilter)
            {
            TestFilter otherFilter = (TestFilter) other;
            return Objects.equals(getAttr(), otherFilter.getAttr()) && Objects.equals(getCount(), otherFilter.getCount());
            }
        return false;
        }

    public int hashCode()
    {
        return Objects.hash(m_sAttr, m_cCount);
    }

    // ----- data members ---------------------------------------------------

    protected String m_sAttr;
    protected int m_cCount;
    }
