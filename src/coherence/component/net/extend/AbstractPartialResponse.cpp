/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPartialResponse.hpp"

#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::pof::PofHelper;
using coherence::util::Map;


// ----- constructors -------------------------------------------------------

AbstractPartialResponse::AbstractPartialResponse()
    : super(), m_vCookie(self())
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractPartialResponse::readExternal(PofReader::Handle hIn)
    {
    AbstractPofResponse::readExternal(hIn);
    setCookie(hIn->readBinary(6));
    }

void AbstractPartialResponse::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofResponse::writeExternal(hOut);
    hOut->writeBinary(6, getCookie());
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPartialResponse::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "Cookie=" << getCookie());
    }


// ----- accessors ----------------------------------------------------------

Binary::View AbstractPartialResponse::getCookie() const
    {
    return m_vCookie;
    }

void AbstractPartialResponse::setCookie(Binary::View vBin)
    {
    m_vCookie = vBin;
    }

COH_CLOSE_NAMESPACE4
