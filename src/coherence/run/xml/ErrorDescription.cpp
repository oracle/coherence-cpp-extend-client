/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/ErrorDescription.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


// ----- constructor --------------------------------------------------------

ErrorDescription::ErrorDescription(size32_t iLineStart, size32_t iLineEnd,
        size32_t ofStart, size32_t ofEnd, Severity severity, String::View vsCode,
        String::View vsMessage)

        : m_iLineStart(iLineStart), m_iLineEnd(iLineEnd), m_ofStart(ofStart),
          m_ofEnd(ofEnd), m_severity(severity), f_vsCode(self(), vsCode),
          f_vsMessage(self(), vsMessage)
    {
    }


// ----- ErrorDescription interface -----------------------------------------

size32_t ErrorDescription::getLine() const
    {
    return m_iLineStart;
    }

size32_t ErrorDescription::getLineEnd() const
    {
    return m_iLineEnd;
    }

size32_t ErrorDescription::getOffset() const
    {
    return m_ofStart;
    }

size32_t ErrorDescription::getOffsetEnd() const
    {
    return m_ofEnd;
    }

size32_t ErrorDescription::getLength() const
    {
    return m_iLineStart == m_iLineEnd ? m_ofEnd - m_ofStart : 0;
    }

ErrorDescription::Severity ErrorDescription::getSeverity() const
    {
    return m_severity;
    }

String::View ErrorDescription::getCode() const
    {
    return f_vsCode;
    }

String::View ErrorDescription::getMessage() const
    {
    return f_vsMessage;
    }

String::View ErrorDescription::severityToString() const
    {
    switch(getSeverity())
        {
        case ErrorDescription::none:
            return "No Error";
        case ErrorDescription::info:
            return "Information";
        case ErrorDescription::warning:
            return "Warning";
        case ErrorDescription::error:
            return "Error";
        case ErrorDescription::fatal:
            return "Fatal Error";
        default:
            return "Unknown Severity";
        }
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ErrorDescription::toString() const
    {
    return COH_TO_STRING("(" << getLine() << ", " << getOffset()
            << ", " << getLength() << ")  " << severityToString() << "  #" << getCode() << ": " << getMessage());
    }

bool ErrorDescription::equals(Object::View v) const
    {
    ErrorDescription::View vThat = cast<ErrorDescription::View>(v, false);

    if (NULL == vThat)
        {
        return false;
        }
    else if (((const ErrorDescription*) this) == vThat)
        {
        return true;
        }
    else if (m_iLineStart  == vThat->m_iLineStart &&
            m_iLineEnd    == vThat->m_iLineEnd   &&
            m_ofStart     == vThat->m_ofStart    &&
            m_ofEnd       == vThat->m_ofEnd      &&
            m_severity    == vThat->m_severity   &&
            Object::equals(getCode(), vThat->getCode()) &&
            Object::equals(getMessage(), vThat->getMessage()))
        {
        return true;
        }
    else
        {
        return false;
        }
    }

size32_t ErrorDescription::hashCode() const
    {
    size32_t nHash = 17;
    nHash = 37 * nHash + m_iLineStart;
    nHash = 37 * nHash + m_iLineEnd;
    nHash = 37 * nHash + m_ofStart;
    nHash = 37 * nHash + m_ofEnd;
    nHash = 37 * nHash + m_severity;
    nHash = 37 * nHash + f_vsCode->hashCode();
    nHash = 37 * nHash + f_vsMessage->hashCode();

    return nHash;
    }

COH_CLOSE_NAMESPACE3
