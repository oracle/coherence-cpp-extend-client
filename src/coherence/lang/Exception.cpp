/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Exception.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/Thread.hpp"

#include "private/coherence/lang/ExceptionStackElement.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <string.h>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::StringHelper;
using coherence::util::logging::Logger;

// see ThrowablePofSerializer for Exception Class registration

// ----- constructors -------------------------------------------------------

Exception::Exception(String::View vsMsg, Exception::View veCause)
    : f_vsMessage(self(), vsMsg), f_vaStackFrames(self()),
      f_vsThreadName(self()), f_veCause(self(), veCause),
      f_vsDescription(self(), NULL, /*fMutable*/ true)
    {
    }

Exception::Exception(const Exception& that)
    : f_vsMessage(self(), that.f_vsMessage),
      f_vaStackFrames(self(), that.f_vaStackFrames),
      f_vsThreadName(self(), that.f_vsThreadName),
      f_veCause(self(), that.f_veCause),
      f_vsDescription(self(), that.f_vsDescription, /*fMutable*/ true)
    {
    }

// ----- Exception interface ------------------------------------------------

String::View Exception::getName() const
    {
    return Class::getClassName(this);
    }

String::View Exception::formatStackTrace() const
    {
    String::View      vs       = "";
    String::View      vsMsg    = getMessage();
    ObjectArray::View vaFrames = cast<ObjectArray::View>(getStackTrace());
    Exception::View   veCause  = getCause();
    String::View      vsThread = getThreadName();

    vs = COH_TO_STRING(vs << getName());
    if (NULL != vsMsg)
        {
        vs = COH_TO_STRING(vs << ": " << vsMsg);
        }
    vs = COH_TO_STRING(vs << std::endl);

    if (NULL == vaFrames)
        {
        vs = COH_TO_STRING(vs << "<no stack frames available>" << std::endl);
        }
    else
        {
        vs = COH_TO_STRING(vs << Thread::formatStackTrace(vaFrames));
        }

    if (NULL != vsThread)
        {
        vs = COH_TO_STRING(vs << "    on thread \"" << getThreadName() << "\"" << std::endl);
        }

    if (NULL != veCause)
        {
        vs = COH_TO_STRING(vs << "Caused by: " << veCause << std::endl);
        }

    return vs;
    }

String::View Exception::getDescription() const
    {
    String::View vsDescription = f_vsDescription;
    if (NULL == vsDescription)
        {
        initialize(f_vsDescription, vsDescription = formatStackTrace());
        }

    return vsDescription;
    }


String::View Exception::getMessage() const
    {
    return f_vsMessage;
    }

Exception::View Exception::getCause() const
    {
    return f_veCause;
    }

void Exception::setMessage(String::View vsMsg)
    {
    initialize(f_vsMessage, vsMsg);
    }

void Exception::setThreadName(String::View vsThreadName)
    {
    initialize(f_vsThreadName, vsThreadName);
    }

String::View Exception::getThreadName() const
    {
    return f_vsThreadName;
    }

void Exception::setStackTrace(ObjectArray::View vaFrames)
    {
    initialize(f_vaStackFrames, vaFrames);
    }

ObjectArray::View Exception::getStackTrace() const
    {
    return f_vaStackFrames;
    }

Exception::Handle Exception::fillInStackTrace(String::View vsFile,
        int32_t nLine, String::View vsFunction)
    {
    ObjectArray::Handle haFrames = Thread::getStackTrace(/*cTrim*/ 1);
    if (vsFile != NULL && nLine != 0 && vsFunction != NULL)
        {
        // agument the thread's trace
        if (NULL == haFrames || haFrames->length == 0)
            {
            haFrames = ObjectArray::create(1);
            }

        // repalce top frame with an exception based frame
        haFrames[0] = ExceptionStackElement::create(vsFile, nLine, vsFunction);
        }
    setStackTrace(haFrames);
    setThreadName(Thread::currentThread()->getName());
    return this;
    }

void Exception::raise() const
    {
    super::raise();
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Exception::toString() const
    {
    return getDescription();
    }


// ----- free functions -----------------------------------------------------

COH_EXPORT void coh_throw(Exception::Holder ohE,
        String::View vsFile, int32_t nLine, String::View vsFunction)
    {
    Exception::Handle hE = cast<Exception::Handle>(ohE, false);
    if (NULL != hE)
        {
        // augment exception with the current stack and thread name
        hE->fillInStackTrace(vsFile, nLine, vsFunction);
        }

    // Log all thrown exceptions
    static bool fLogging = false;
    if (!fLogging)
        {
        // avoid the possibility of endless recursion if the logger was
        // to throw here.  Note this is not thread safe, and if two threads
        // are throwing exceptions at the same time some are likely to get
        // missed.  The primary intent of the logging is to allow us to
        // identify exceptions thrown during static init as they cannot be
        // caught.  At that point we are single threaded so it would not
        // matter.
        fLogging = true;
        try
            {
            COH_LOG("Throwing: " << ohE, Logger::level_d9);
            }
        catch (...)
            {
            fLogging = false;
            throw;
            }
        fLogging = false;
        }

    ohE->raise();
    throw 0;
    }

COH_CLOSE_NAMESPACE2
