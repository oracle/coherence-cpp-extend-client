/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOOK_HPP
#define COH_BOOK_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Book describes a published book.
*
* The type implements PortableObject for efficient cross-platform
* serialization.
*
* @author par  2013.05.07
*/
class Book
    : public class_spec<Book,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Book>;

    protected:
        Book()
            : m_vsAuthor(self(), String::null_string),
              m_vsTitle (self(), String::null_string),
              m_vsISBN  (self(), String::null_string),
              m_nPages  (0),
              m_nYear   (0)
            {
            }

        /**
        * Construct a Book.
        *
        * @param vsAuthor  author of the book
        * @param vsTitle   title of the book
        * @param vsISBN    ISBN of the book
        * @param nPages    number of pages
        * @param nYear     year published
        */
        Book(String::View vsAuthor, String::View vsTitle,
                String::View vsISBN, int nPages, int nYear)
            : m_vsAuthor(self(), vsAuthor), m_vsTitle(self(), vsTitle),
              m_vsISBN(self(), vsISBN), m_nPages(nPages), m_nYear(nYear)
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setAuthor(hIn->readString(0));
            setTitle (hIn->readString(1));
            setISBN  (hIn->readString(2));
            setPages (hIn->readInt32(3));
            setYear  (hIn->readInt32(4));
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeString(0, getAuthor());
            hOut->writeString(1, getTitle());
            hOut->writeString(2, getISBN());
            hOut->writeInt32 (3, getPages());
            hOut->writeInt32 (4, getYear());
            }

    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        bool equals(Object::View v) const
            {
            if (this == v)
                {
                return true;
                }

            Book::View vThat = cast<Book::View>(v, false);
            if (NULL != vThat)
                {
                return Object::equals(this->getAuthor(), vThat->getAuthor())
                    && Object::equals(this->getTitle(),  vThat->getTitle())
                    && Object::equals(this->getISBN(),   vThat->getISBN())
                    && this->getPages() == vThat->getPages()
                    && this->getYear() == vThat->getYear();
                }
            return false;
            }


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Author.
        *
        * @return the author
        */
        String::View getAuthor() const
            {
            return m_vsAuthor;
            }

        /**
        * Set the Author.
        *
        * @param vsAuthor  the author
        */
        void setAuthor(String::View vsAuthor)
            {
            m_vsAuthor = vsAuthor;
            }

        /**
        * Return the Title.
        *
        * @return the title
        */
        String::View getTitle() const
            {
            return m_vsTitle;
            }

        /**
        * Set the title.
        *
        * @param vsTitle  the title
        */
        void setTitle(String::View vsTitle)
            {
            m_vsTitle = vsTitle;
            }

        /**
        * Return the ISBN.
        *
        * @return the ISBN
        */
        String::View getISBN() const
            {
            return m_vsISBN;
            }

        /**
        * Set the ISBN.
        *
        * @param vsISBN  the ISBN
        */
        void setISBN(String::View vsISBN)
             {
             m_vsISBN = vsISBN;
             }

        /**
        * Return the number of pages.
        *
        * @return the number of pages
        */
        int getPages() const
            {
            return m_nPages;
            }

        /**
        * Set the number of pages.
        *
        * @param nPages  the number of pages
        */
        void setPages(int nPages)
            {
            m_nPages = nPages;
            }

        /**
        * Set the year published.
        *
        * @return the year published
        */
        int getYear() const
            {
            return m_nYear;
            }

        /**
        * Set the year published.
        *
        * @param nYear the year published
        */
        void setYear(int nYear)
            {
            m_nYear = nYear;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Author.
        */
        MemberView<String> m_vsAuthor;

        /**
        * Title.
        */
        MemberView<String> m_vsTitle;

        /**
        * ISBN.
        */
        MemberView<String> m_vsISBN;

        /**
        * Number of pages.
        */
        int m_nPages;

        /**
        * Year published.
        */
        int m_nYear;
    };

COH_REGISTER_PORTABLE_CLASS(1201, Book);

COH_CLOSE_NAMESPACE2

#endif // COH_BOOK_HPP
