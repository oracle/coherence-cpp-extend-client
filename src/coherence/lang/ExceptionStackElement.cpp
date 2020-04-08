/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/lang/ExceptionStackElement.hpp"

#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::StringHelper;


// ----- constructors -------------------------------------------------------

ExceptionStackElement::ExceptionStackElement(String::View vsFile, int32_t nLine,
        String::View vsFunction)
        : f_vsFile(self(), StringHelper::stripSource(vsFile)), m_nLine(nLine),
          f_vsFunction(self(), vsFunction)
    {
    }


// ----- ExceptionStackElement interface ------------------------------------

String::View ExceptionStackElement::getFile() const
    {
    return f_vsFile;
    }

int32_t ExceptionStackElement::getLine() const
    {
    return m_nLine;
    }

String::View ExceptionStackElement::getFunction() const
    {
    return f_vsFunction;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ExceptionStackElement::toString() const
    {
    return COH_TO_STRING("at " << getFunction() << "(" << getFile() << ':' << getLine() << ')');
    }

COH_CLOSE_NAMESPACE2
