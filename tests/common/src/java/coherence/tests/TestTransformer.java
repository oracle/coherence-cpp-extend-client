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
import com.tangosol.util.ValueExtractor;

import java.io.IOException;

import java.util.Map;
import java.util.Objects;

/**
 * Simple ValueExtractor for validation of custom "Transformers" with view-scheme configurations.
 * Wouldn't recommend its use elsewhere; it doesn't do much.
 *
 * @author rl 2019.05.14
 * @since 12.2.1.4
 */ 
public class TestTransformer
        implements ValueExtractor, PortableObject
    {
    // ----- constructors ---------------------------------------------------

    public TestTransformer()
        {
        this(-1);
        }

    public TestTransformer(int count)
        {
        m_cCount = -1;
        }

    // ----- ValueExtractor interface --------------------------------------8

    public Object extract(Object o)
        {
        return null;
        }

    // ----- accessors ------------------------------------------------------

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
        m_cCount = reader.readInt(0);
        }

    @Override
    public void writeExternal(PofWriter writer) throws IOException
        {
        writer.writeInt(0, m_cCount);
        }

    // ----- object methods -------------------------------------------------

    public boolean equals(Object other)
        {
        if (other instanceof TestTransformer)
            {
            TestTransformer otherTrans = (TestTransformer) other;
            return Objects.equals(getCount(), otherTrans.getCount());
            }
        return false;
        }

    public int hashCode()
    {
        return Objects.hash(m_cCount);
    }

    // ----- data members ---------------------------------------------------

    protected String m_sAttr;
    protected int m_cCount;
    }
