/*
 * Copyright (c) 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;

import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;
import com.tangosol.io.pof.PortableObject;

import com.tangosol.net.CacheFactory;

import com.tangosol.util.Binary;
import com.tangosol.util.BinaryEntry;
import com.tangosol.util.ExternalizableHelper;
import com.tangosol.util.InvocableMap;

import com.tangosol.util.processor.AbstractProcessor;

import java.io.IOException;

import java.nio.ByteBuffer;

import java.nio.charset.StandardCharsets;

/**
 * The TestUtf8Processor is used to validate UTF-8 serialization.
 *
 * @author phf 2024.08.27
 * @since 14.1.2.0.0
 */
public class TestUtf8Processor
        extends AbstractProcessor
        implements PortableObject
    {
    // ----- constructors ---------------------------------------------------

    public TestUtf8Processor()
        {
        }

    // ----- EntryProcessor implementation ----------------------------------

    @Override
    public Object process(InvocableMap.Entry entry)
        {
        // string initialization copied from Java ExternalizableHelperTest
        String       sExpected   = new String(toBytes(new int[] {0xf0938080, 0xf09f8ebf, 0xf09f8f80, 0xf09f8e89, 0xf09f9294}),
                                              StandardCharsets.UTF_8);
        BinaryEntry  binEntry    = (BinaryEntry) entry;
        Binary       binValue    = ExternalizableHelper.getUndecorated(binEntry.getBinaryValue());
        Binary       binExpected = ExternalizableHelper.getUndecorated((Binary) binEntry.getContext()
                                                                                        .getValueToInternalConverter()
                                                                                        .convert(sExpected));

        // use CacheFactory instead of Logger to be able to test with older Coherence versions
        CacheFactory.log("TestUtf8Processor: this entry's binary value: " + binValue, CacheFactory.LOG_INFO);
        CacheFactory.log("TestUtf8Processor: serialized sExpected     : " + binExpected, CacheFactory.LOG_INFO);

        // Logger.info("*** this entry's deserialized value: " + entry.getValue());
        // Logger.info("*** sExpected                      : " + sExpected);

        // Coherence versions prior to 14.1.2 will use the old encoding
        boolean fVersionCompatible = binEntry.getContext().getCacheService().isVersionCompatible(14,1,2,0,0);
        CacheFactory.log("TestUtf8Processor: isVersionCompatible(14,1,2,0,0)=" + fVersionCompatible);

        return Boolean.valueOf(fVersionCompatible ? binExpected.equals(binValue) : true);
        }

    // ----- PortableObject implementation ----------------------------------

    @Override
    public void readExternal(PofReader in)
            throws IOException
        {
        }

    @Override
    public void writeExternal(PofWriter out)
            throws IOException
        {
        }

    // ----- helper methods -------------------------------------------------

    private static byte[] toBytes(int[] ai)
        {
        ByteBuffer buf = ByteBuffer.allocate(4 * ai.length);
        for (int n : ai)
            {
            buf.putInt(n);
            }
        return buf.array();
        }

    // ----- data members ---------------------------------------------------

    private int m_nId;
    }
