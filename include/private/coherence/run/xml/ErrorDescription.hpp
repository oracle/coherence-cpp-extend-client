/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ERROR_DESCRIPTION_HPP
#define COH_ERROR_DESCRIPTION_HPP

#include "coherence/lang.ns"


COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* Description of a a parse error.
*
* @author js  2008.01.03
*/
class COH_EXPORT ErrorDescription
    : public class_spec<ErrorDescription>
    {
    friend class factory<ErrorDescription>;

    // ----- enums ----------------------------------------------------------

    public:
        /**
        * The severity of the error.
        */
        enum Severity { none, info, warning, error, fatal };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ErrorDescription
        *
        * @param iLineStart  the first line containing the error
        * @param iLineEnd    the ending line containing the error
        * @param ofStart     the offset of the error within the line
        * @param ofEnd       the ending offset
        * @param severity    the severity of the error
        * @param vsCode      the error code
        * @param vsMessage   the error message
        */
        ErrorDescription(size32_t iLineStart, size32_t iLineEnd, size32_t ofStart,
                size32_t ofEnd, Severity severity, String::View vsCode,
                String::View vsMessage);


    // ----- ErrorDescription interface -------------------------------------

    public:
        /**
        * @return the first line in which the error was detected
        */
        virtual size32_t getLine() const;

        /**
        * @return the last line in which the error was detected
        */
        virtual size32_t getLineEnd() const;

        /**
        * @return the starting offset of the error within the line
        */
        virtual size32_t getOffset() const;

        /**
        * @return the ending offset of the error
        */
        virtual size32_t getOffsetEnd() const;

        /**
        * @return the length of the error or 0 if unable to determine the
        *         length due to the error spanning lines
        */
        virtual size32_t getLength() const;

        /**
        * @return the severity of the error
        */
        virtual Severity getSeverity() const;

        /**
        * @return the error code
        */
        virtual String::View getCode() const;

        /**
        * @return the error message
        */
        virtual String::View getMessage() const;


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Return the severity of the error as a string
        */
        virtual String::View severityToString() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- Data members ---------------------------------------------------

    protected:
        /**
        * Line in which the error was detected.
        */
        size32_t m_iLineStart;

        /**
        * Ending line in which the error was detected.
        */
        size32_t m_iLineEnd;

        /**
        * Offset of the error within the line.
        */
        size32_t m_ofStart;

        /**
        * Ending offset.
        */
        size32_t m_ofEnd;

        /**
        * Severity of the error.
        */
        Severity m_severity;

        /**
        * Error Code.
        */
        FinalView<String> f_vsCode;

        /**
        * Error Message.
        */
        FinalView<String> f_vsMessage;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ERROR_DESCRIPTION_HPP
