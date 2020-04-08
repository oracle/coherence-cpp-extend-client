/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_SCRIPT_HPP
#define COH_SIMPLE_SCRIPT_HPP

#include "private/coherence/dev/compiler/Script.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* Implements a simple ASCII script object.
*
* @author tb  2007.12.13
*/
class COH_EXPORT SimpleScript
    : public class_spec<SimpleScript,
        extends<Object>,
        implements<Script> >
    {
    friend class factory<SimpleScript>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SimpleScript instance.
        */
        SimpleScript();

        /**
        * Create a new SimpleScript instance from the given string.
        *
        * @param vsScript  the script
        */
        SimpleScript(String::View vsScript);

    private:
        /**
        * Blocked copy constructor.
        */
        SimpleScript(const SimpleScript&);


    // ----- Script interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void setScript(String::View vsScript);

        /**
        * {@inheritDoc}
        */
        virtual bool hasMoreChars() const;

        /**
        * {@inheritDoc}
        */
        virtual char nextChar();

        /**
        * {@inheritDoc}
        */
        virtual void putBackChar();

        /**
        * {@inheritDoc}
        */
        virtual ParsePosition::Handle savePosition() const;

        /**
        * {@inheritDoc}
        */
        virtual void restorePosition(ParsePosition::View vParsePos);

        /**
        * {@inheritDoc}
        */
        virtual size32_t getLine() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getOffset() const;

        /**
        * {@inheritDoc}
        */
        virtual Script::Handle subScript(ParsePosition::View vParsePos) const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getText() const;

    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The array of characters that make up the script.
        */
        FinalView<Array<octet_t> > f_achScript;

        /**
        * The current character offset within the script.
        */
        size32_t m_ofCurrent;

        /**
        * The current line number within the script.
        */
        size32_t m_iLine;

        /**
        * The current character offset with the current line.
        */
        size32_t m_ofInLine;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_SCRIPT_HPP
