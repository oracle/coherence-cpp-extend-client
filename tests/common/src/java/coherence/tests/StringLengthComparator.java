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

import java.util.Comparator;


/**
* Comparator used for testing ComparableMax and ComparableMin.
* 
* @author pp  2009.01.16
*/
public class StringLengthComparator 
		implements Comparator, PortableObject
	{	
	// ----- constructors ---------------------------------------------------

    /**
    * Default constructor.
    */
	public StringLengthComparator()
		{
		}
	
	
	// ----- Comparator interface -------------------------------------------
	
	/**
	* Compare two strings based on their length
	* 
	* @throws NullPointerException if either argument is null      
	*/
	public int compare(Object o1, Object o2)
		{
		return ((String) o1).length() - ((String) o2).length();
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
	}