/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/logging/Standard.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <fstream>
#include <iostream>

COH_OPEN_NAMESPACE3(coherence,util,logging)


// ----- constructors/destructors -------------------------------------------

Standard::Standard()
    : m_ostream(&std::cerr)
    {
    }

Standard::~Standard()
    {
    std::ostream* pOut = m_ostream;
    if (NULL != pOut && pOut != &std::cout &&
        pOut != &std::cerr && pOut != &std::clog)
        {
        m_ostream = NULL;
        delete pOut;
        }
    }


// ----- LogOutput interface ------------------------------------------------

void Standard::configure(OperationalContext::View vContext)
    {
    if (m_ostream != &std::cerr)
        {
        COH_THROW (UnsupportedOperationException::create("already configured"));
        }

    String::View vsOut = vContext->getLogDestination();

    if (vsOut == NULL || vsOut->length() == 0 || vsOut->equals("stderr"))
        {
        m_ostream = &std::cerr;
        }
    else if (vsOut->equals("stdout"))
        {
        m_ostream = &std::cout;
        }
    else if (vsOut->equals("stdlog"))
        {
        m_ostream = &std::clog;
        }
    else // file
        {
        std::ofstream* pOut = new std::ofstream(vsOut->getCString(),
                std::ofstream::out | std::ofstream::app | std::ofstream::ate);
        if (pOut->fail() || pOut->bad())
            {
            delete pOut;
            m_ostream = &std::cerr;
            COH_LOG("Failed to open log file: \"" << vsOut
                    << "\", reverting to stderr", -1);
            }
        else
            {
            m_ostream = pOut;
            }
        }
    }

void Standard::write(Object::View voLevel, Exception::View vEx, String::View vsMessage)
    {
    //The .Net LogOutput code synchronized around the stream. If you have a
    // single worker thread is doing the work is this necessary?
    write(voLevel, vsMessage);
    write(voLevel, vEx);
    }

void Standard::write(Object::View /*voLevel*/, Exception::View vEx)
    {
    // TODO NSA: use voLevel
    std::ostream* stream = m_ostream;
    if (stream)
        {
        *stream << vEx << std::endl;
        }
    }

void Standard::write(Object::View /*voLevel*/, String::View vsMessage)
    {
    // TODO NSA: use voLevel
    std::ostream* stream = m_ostream;
    if (NULL != vsMessage && stream)
        {
        *stream << vsMessage << std::endl;
        }
    }

Object::View Standard::translateLevel(int32_t nLevel) const
    {
    return Integer32::valueOf(nLevel); // TODO NSA We need to replace this with the equivalent of Base::makeInteger
    }

void Standard::close()
    {
    // finally construct
    struct Finally
        {
        Finally(Standard::Handle _hThis)
            : hThis(_hThis)
            {
            }

        ~Finally()
            {
            std::ostream* pOut = hThis->m_ostream;
            if (NULL != pOut && pOut != &std::cout &&
                pOut != &std::cerr && pOut != &std::clog)
                {
                hThis->m_ostream = NULL;
                delete pOut;
                }
            }

        Standard::Handle hThis;
        } finally(this);

    m_ostream->flush();
    }


// ----- Standard interface -------------------------------------------------

std::ostream* Standard::getOutputStream()
    {
    return m_ostream;
    }

void Standard::setOutputStream(std::ostream* stream)
    {
    // For testing only, this method is not thread safe, and leaves memory
    // management of the old stream up to the caller
    m_ostream = stream;
    }

COH_CLOSE_NAMESPACE3 //COH_OPEN_NAMESPACE3(coherence,util,logging)
