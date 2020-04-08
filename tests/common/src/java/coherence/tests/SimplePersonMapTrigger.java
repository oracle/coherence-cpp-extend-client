/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
package coherence.tests;

import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;
import com.tangosol.io.pof.PortableObject;

import com.tangosol.util.MapEvent;
import com.tangosol.util.MapListener;
import com.tangosol.util.MapTrigger;

import java.io.IOException;

/**
 * Trigger class to modify value being put into the cache.
 * Changes last name of SimplePerson instance to all upper case.
 *
 * @author par  2013.09.23
 *
 * @since Coherence 12.1.3
 */
public class SimplePersonMapTrigger
        implements MapTrigger, PortableObject
    {
    /*
     * Default constructor.
     */
    public SimplePersonMapTrigger()
        {
        }

    /**
     * {@inheritDoc}
     */
    public void process(MapTrigger.Entry entry)
        {
        SimplePerson person  = (SimplePerson) entry.getValue();
        String sName         = person.getLastName();
        String sNameUC       = sName.toUpperCase();

        if (!sNameUC.equals(sName))
            {
            person.setLastName(sNameUC);
            entry.setValue(person);
            }
        }

    // ----- PortableObject implementation ----------------------------

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

    // ---- Object implementation -------------------------------------

    /**
     * {@inheritDoc}
     */
    public boolean equals(Object o)
        {
        return o != null && o.getClass() == this.getClass();
        }

    /**
     * {@inheritDoc}
     */
    public int hashCode()
        {
        return getClass().getName().hashCode();
        }
    }
