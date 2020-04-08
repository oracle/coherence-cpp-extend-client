/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/dev/compiler/SimpleScript.hpp"

#include "coherence/io/EOFException.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)

using coherence::io::EOFException;


// ----- local class: Position ----------------------------------------------

/**
* Stores all information required to later restore the current position in
* the script.
*
* @author tb 2007.01.17
*/
class Position
    : public cloneable_spec<Position,
        extends<Object>,
        implements<ParsePosition> >
    {
    friend class factory<Position>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
        * Create a new Position instance.
        *
        * @param vsScript   the array of characters that make up the script
        * @param ofCurrent  the current character offset within the script
        * @param iLine      the current line number within the script
        * @param ofInLine   the current character offset in the current line
        */
        Position(Script::View vsScript, size32_t ofCurrent, size32_t iLine,
                size32_t ofInLine)
            : f_vScript(self(), vsScript),
              m_ofCurrent(ofCurrent),
              m_iLine(iLine),
              m_ofInLine(ofInLine)
            {
            }

        /**
        * Copy constructor.
        */
        Position(const Position& that)
            : super(that), f_vScript(self(), that.f_vScript),
              m_ofCurrent(that.m_ofCurrent),
              m_iLine(that.m_iLine),
              m_ofInLine(that.m_ofInLine)
            {
            }

    // ----- data members -----------------------------------------------

    public:
        /**
        * The associated script.
        */
        FinalView<Script> f_vScript;

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


// ----- constructors -------------------------------------------------------

SimpleScript::SimpleScript()
        : f_achScript(self()),
          m_ofCurrent(0),
          m_iLine(0),
          m_ofInLine(0)
    {
    }

SimpleScript::SimpleScript(String::View vsScript)
        : f_achScript(self()),
          m_ofCurrent(0),
          m_iLine(0),
          m_ofInLine(0)
    {
    setScript(vsScript);
    }


// ----- Script interface ---------------------------------------------------

void SimpleScript::setScript(String::View vsScript)
    {
    // this method may only be called once
    COH_ENSURE(is_null(f_achScript));

    if (is_null(vsScript) || !vsScript->isASCII() )
        {
        COH_THROW (IllegalArgumentException::create(
                "The supplied script must be ASCII."));
        }

    initialize(f_achScript, vsScript->getOctets());
    }

bool SimpleScript::hasMoreChars() const
    {
    Array<octet_t>::View achScript = f_achScript;
    if (NULL == achScript)
        {
        return false;
        }
    // - 1 from length to account for NULL terminator
    return (m_ofCurrent < achScript->length - 1);
    }

char SimpleScript::nextChar()
    {
    Array<octet_t>::View achScript = f_achScript;
    // - 1 from length to account for NULL terminator
    size32_t          cchScript = achScript->length - 1;
    char              ch;

    try
        {
        ch = achScript[m_ofCurrent++];
        }
    catch (IndexOutOfBoundsException::View)
        {
        COH_THROW (EOFException::create());
        }

    if ('\n' == ch  || '\r' == ch)
        {
        bool fMore = (m_ofCurrent < cchScript);

        // if the current character is a carriage return, it increments
        // the line counter
        if ('\r' == ch && fMore && '\n' == achScript[m_ofCurrent])
            {
            // this carriage return does not increment the line number
            // since it is followed by a new line character
            ++m_ofInLine;
            }
        else
            {
            // advance to the next line
            m_ofInLine = 0;
            ++m_iLine;
            }
        }
    else
        {
        if (Script::sub == ch && m_ofCurrent < cchScript)
            {
            // Sub (�3.5) - can only appear as the last character
            --m_ofCurrent;
            COH_THROW (IllegalStateException::create());
            }

        // advance the offset in the current line
        ++m_ofInLine;
        }
    return ch;
    }

void SimpleScript::putBackChar()
    {
    if (m_ofCurrent == 0 || m_ofCurrent == String::npos)
        {
        COH_THROW (IllegalStateException::create());
        }

    Array<octet_t>::View achScript = f_achScript;
    // - 1 from length to account for NULL terminator
    size32_t          cch       = achScript->length - 1;

    COH_ENSURE(m_ofCurrent <= cch);

    size32_t          ofCurrent = --m_ofCurrent;
    char              ch        = achScript[ofCurrent];

    if ('\n' == ch || '\r' == ch)
        {
        // if the regurgiated character is a carriage return, it decrements
        // the line counter unless a line feed immediately follows it
        if ('\r' == ch &&
                (ofCurrent + 1 < cch && '\n' == achScript[ofCurrent+1]))
            {
            // this carriage return did not increment the line number
            // since it is followed by a new line character
            --m_ofInLine;
            }
        else
            {
            // retreat to the end of the previous line
            --m_iLine;
            m_ofInLine = 0;

            size32_t of        = ofCurrent == 0 ? 0 : ofCurrent - 1;
            while (!(achScript[of] == '\r' || achScript[of] ==   '\n'))
                {
                ++m_ofInLine;
                if (of == 0) break;
                --of;
                }
            }
        }
    else
        {
        // retreat the offset in the current line
        --m_ofInLine;
        }
    }

ParsePosition::Handle SimpleScript::savePosition() const
    {
    Position::Handle hPos = Position::create(this, m_ofCurrent, m_iLine,
            m_ofInLine);

    return hPos;
    }

void SimpleScript::restorePosition(ParsePosition::View vParsePos)
    {
    Position::View vPos = cast<Position::View>(vParsePos);

    if (vPos->f_vScript != this)
        {
        COH_THROW (IllegalArgumentException::create(
                "Unknown ParsePosition object"));
        }
    m_ofCurrent = vPos->m_ofCurrent;
    m_iLine     = vPos->m_iLine;
    m_ofInLine  = vPos->m_ofInLine;
    }

size32_t SimpleScript::getLine() const
    {
    return m_iLine;
    }

size32_t SimpleScript::getOffset() const
    {
    return m_ofInLine;
    }

Script::Handle SimpleScript::subScript(ParsePosition::View vParsePos) const
    {
    if (NULL == vParsePos)
        {
        COH_THROW (IllegalArgumentException::create(
                "Illegal ParsePosition object!"));
        }

    Position::View vPos = cast<Position::View>(vParsePos);
    if (vPos->f_vScript != this)
        {
        COH_THROW (IllegalArgumentException::create(
                "Invalid ParsePosition object!"));
        }

    size32_t of        = vPos->m_ofCurrent;
    size32_t ofCurrent = m_ofCurrent;
    size32_t cch       = ofCurrent - of;

    COH_ENSURE(ofCurrent > of);
    String::View vsSub = getText()->substring(of, of + cch);
    return SimpleScript::create(vsSub);
    }

String::View SimpleScript::getText() const
    {
    // - 1 from length to account for NULL terminator
    return String::create(f_achScript, 0, f_achScript->length - 1);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SimpleScript::toString() const
    {
    return COH_TO_STRING(f_achScript);
    }

COH_CLOSE_NAMESPACE3
