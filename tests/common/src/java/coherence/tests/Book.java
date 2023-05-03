/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
package coherence.tests;

import java.io.IOException;
import java.io.Serializable;

import com.tangosol.io.pof.PofReader;
import com.tangosol.io.pof.PofWriter;
import com.tangosol.io.pof.PortableObject;

import com.tangosol.util.Base;
public class Book
        extends Base
        implements PortableObject, Serializable
    {
    // ----- constructors ---------------------------------------------------

    public Book()
        {
        }

    public Book(String sAuthor, String sTitle, String sISBN, int nPages, int nYear)
        {
        m_sAuthor = sAuthor;
        m_sTitle  = sTitle;
        m_sISBN   = sISBN;
        m_nPages  = nPages;
        m_nYear   = nYear;
        }

    // ----- accessors ------------------------------------------------------

    public String getAuthor()
        {
        return m_sAuthor;
        }

    public void setCity(String sAuthor)
        {
        m_sAuthor = sAuthor;
        }

    public String getTitle()
        {
        return m_sTitle;
        }

    public void setTitle(String sTitle)
        {
        m_sTitle = sTitle;
        }

    public String getISBN()
        {
        return m_sISBN;
        }

    public void setISBN(String sISBN)
        {
        m_sISBN = sISBN;
        }

    public int getPages()
        {
        return m_nPages;
        }

    public void setPages(int nPages)
        {
        m_nPages = nPages;
        }

    public int getYear()
        {
        return m_nYear;
        }

    public void setYear(int nYear)
        {
        m_nYear = nYear;
        }

    // ----- PortableObject interface ---------------------------------------

    public void readExternal(PofReader reader)
        throws IOException
        {
        m_sAuthor = reader.readString(AUTHOR);
        m_sTitle  = reader.readString(TITLE);
        m_sISBN   = reader.readString(ISBN);
        m_nPages  = reader.readInt(PAGES);
        m_nYear   = reader.readInt(YEAR);
        }

    public void writeExternal(PofWriter writer)
        throws IOException
        {
        writer.writeString(AUTHOR, m_sAuthor);
        writer.writeString(TITLE,  m_sTitle);
        writer.writeString(ISBN,   m_sISBN);
        writer.writeInt(PAGES,   m_nPages);
        writer.writeInt(YEAR,    m_nYear);
        }

    // ----- Object interface ---------------------------------------

    public String toString()
        {
            return "Book {m_sAuthor="
            + m_sAuthor + ", m_sTitle=" + m_sTitle + ", m_sISBN="
            + m_sISBN + ", m_nPages=" + m_nPages
            + ", m_nYear=" + m_nYear + "}";
        }

    public boolean equals(Object o)
        {
        if (o == this)
            {
            return true;
            }

        if (o instanceof Book)
            {
            Book that = (Book) o;
            return equals(this.m_sAuthor, that.m_sAuthor)
               &&  equals(this.m_sTitle,  that.m_sTitle)
               &&  equals(this.m_sISBN,   that.m_sISBN)
               &&  this.m_nPages ==       that.m_nPages
               &&  this.m_nYear  ==       that.m_nYear;
            }

        return false;
        }

    // ----- data members ---------------------------------------------------

    public String m_sAuthor;
    public String m_sTitle;
    public String m_sISBN;
    public int    m_nPages;
    public int    m_nYear;

    public static final int AUTHOR = 0;
    public static final int TITLE  = 1;
    public static final int ISBN   = 2;
    public static final int PAGES  = 3;
    public static final int YEAR   = 4;
    }
