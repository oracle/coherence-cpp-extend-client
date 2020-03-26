/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/SimpleDocument.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


COH_REGISTER_PORTABLE_CLASS(500, SimpleDocument);

// ----- constructors -------------------------------------------------------

SimpleDocument::SimpleDocument(String::View vsName)
    : super(vsName),
      f_vsDtdUri(self(), NULL),
      f_vsDtdName(self(), NULL),
      f_vsEncoding(self(), NULL),
      m_vsDocComment(self(), NULL)
    {
    }

SimpleDocument::SimpleDocument(String::View vsName, String::View vsDtdUri,
                               String::View vsDtdName)
    : super(vsName),
      f_vsDtdUri(self(), vsDtdUri),
      f_vsDtdName(self(), vsDtdName),
      f_vsEncoding(self(), NULL),
      m_vsDocComment(self(), NULL)
    {
    }

SimpleDocument::SimpleDocument(const SimpleDocument& that)
    : super(that),
      f_vsDtdUri(self(), that.f_vsDtdUri),
      f_vsDtdName(self(), that.f_vsDtdName),
      f_vsEncoding(self(), that.f_vsEncoding),
      m_vsDocComment(self(), that.m_vsDocComment)
    {
    }

// ----- SimpleDocument interface -------------------------------------------

String::View SimpleDocument::getDtdUri() const
    {
    return f_vsDtdUri;
    }

void SimpleDocument::setDtdUri(String::View vsUri)
    {
    initialize(f_vsDtdUri, vsUri);
    }

String::View SimpleDocument::getDtdName() const
    {
    return f_vsDtdName;
    }

void SimpleDocument::setDtdName(String::View vsDtdName)
    {
    if (NULL != vsDtdName && vsDtdName->length() == 0)
        {
        vsDtdName = NULL;
        }
    initialize(f_vsDtdName, vsDtdName);
    }

String::View SimpleDocument::getEncoding() const
    {
    return f_vsEncoding;
    }

void SimpleDocument::setEncoding(String::View vsEncoding)
    {
    if (NULL != vsEncoding && vsEncoding->length() == 0)
        {
        vsEncoding = NULL;
        }
    initialize(f_vsEncoding, vsEncoding);
    }

String::View SimpleDocument::getDocumentComment() const
    {
    return m_vsDocComment;
    }

void SimpleDocument::setDocumentComment(String::View vsComment)
    {
    if (NULL != vsComment)
        {
        if (vsComment->length() == 0)
            {
            vsComment = NULL;
            }
        else if (vsComment->indexOf("--") != String::npos)
            {
            COH_THROW (IllegalArgumentException::create(
                "comment contains \"--\"; see XML 1.0 2ed section 2.5 [15]"));
            }
        }
    m_vsDocComment = vsComment;
    }

String::View SimpleDocument::formatXml(bool fPretty, size32_t cIndent) const
    {
    String::View vsDtdUri   = getDtdUri();
    String::View vsDtdName  = getDtdName();
    String::View vsEncoding = getEncoding();
    String::View vsComment  = getDocumentComment();

    String::View vs = "<?xml version='1.0'";
    if (NULL != vsEncoding && vsEncoding->length() > 0)
        {
        vs = COH_TO_STRING(vs << " encoding=" << XmlHelper::quote(vsEncoding));
        }
    vs = COH_TO_STRING(vs << "?>");

    if (fPretty)
        {
        vs = COH_TO_STRING(vs << std::endl);
        }

    if (NULL != vsDtdUri && vsDtdUri->length() > 0)
        {
        vs = COH_TO_STRING(vs << "<!DOCTYPE " << getName() << ' ');
        if (NULL != vsDtdName && vsDtdName->length() > 0)
            {
            vs = COH_TO_STRING(vs << "PUBLIC");
            if (fPretty)
                {
                vs = COH_TO_STRING(vs << std::endl);
                }
            vs = COH_TO_STRING(vs << ' ' << XmlHelper::quote(vsDtdName));
            }
        else
            {
            vs = COH_TO_STRING(vs << "SYSTEM");
            }

        if (fPretty)
            {
            vs = COH_TO_STRING(vs << std::endl);
            }

        vs = COH_TO_STRING(vs << ' ' << XmlHelper::quote(XmlHelper::encodeUri(
                vsDtdUri)) << '>');

        if (fPretty)
            {
            vs = COH_TO_STRING(vs << std::endl);
            }
        else
            {
            vs = COH_TO_STRING(vs << ' ');
            }
        }
    if (NULL != vsComment && vsComment->length() > 0)
        {
        vs = COH_TO_STRING(vs << "<!--");
        if (fPretty)
            {
            vs = COH_TO_STRING(vs << std::endl << XmlHelper::breakLines(vsComment, 78, cIndent));
            }
        else
            {
            vs = COH_TO_STRING(vs << vsComment);
            }

        vs = COH_TO_STRING(vs << "-->");
        if (fPretty)
            {
            vs = COH_TO_STRING(vs << std::endl);
            }
        else
            {
            vs = COH_TO_STRING(vs << ' ');
            }
        }

    vs = COH_TO_STRING(vs << super::formatXml(fPretty, 2));
    if (fPretty)
        {
        vs = COH_TO_STRING(vs << std::endl);
        }

    return vs;
    }

String::View SimpleDocument::getXml() const
    {
    return formatXml();
    }


// ----- PortableObject interface -------------------------------------------

void SimpleDocument::readExternal(PofReader::Handle hIn)
    {
    if (f_vsDtdUri != NULL || f_vsDtdName != NULL || f_vsEncoding != NULL || m_vsDocComment != NULL)
        {
        COH_THROW (IllegalArgumentException::create(""));
        }

    super::readExternal(hIn);

    if (hIn->readBoolean(12))
      {
      initialize(f_vsDtdUri, hIn->readString(13));
      }

    if (hIn->readBoolean(14))
        {
        initialize(f_vsDtdName, hIn->readString(15));
        }

    if (hIn->readBoolean(16))
        {
        initialize(f_vsEncoding, hIn->readString(17));
        }

    if (hIn->readBoolean(18))
        {
        m_vsDocComment = hIn->readString(19);
        }
    }

void SimpleDocument::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    String::View vsDtdUri   = f_vsDtdUri;
    String::View vsDtdName  = f_vsDtdName;
    String::View vsEncoding = f_vsEncoding;
    String::View vsComment  = m_vsDocComment;

    bool fDtdUri   = vsDtdUri   != NULL;
    bool fDtdName  = vsDtdName  != NULL;
    bool fEncoding = vsEncoding != NULL;
    bool fComment  = vsComment  != NULL;

    hOut->writeBoolean(12, fDtdUri);
    if (fDtdUri)
        {
        hOut->writeString(13, f_vsDtdUri);
        }

    hOut->writeBoolean(14, fDtdName);
    if (fDtdName)
        {
        hOut->writeString(15, f_vsDtdName);
        }

    hOut->writeBoolean(16, fEncoding);
    if (fEncoding)
        {
        hOut->writeString(17, f_vsEncoding);
        }

    hOut->writeBoolean(18, fComment);
    if (fComment)
        {
        hOut->writeString(19, m_vsDocComment);
        }
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SimpleDocument::toString() const
    {
    return formatXml(true);
    }

size32_t SimpleDocument::hashCode() const
    {
    String::View vsDtdUri     = f_vsDtdUri;
    String::View vsDtdName    = f_vsDtdName;
    String::View vsEncoding   = f_vsEncoding;
    String::View vsDocComment = m_vsDocComment;

    size32_t nHash = SimpleValue::hashCode();

    nHash = 37 * nHash + Object::hashCode(vsDtdUri);
    nHash = 37 * nHash + Object::hashCode(vsDtdName);
    nHash = 37 * nHash + Object::hashCode(vsEncoding);
    nHash = 37 * nHash + Object::hashCode(vsDocComment);

    return nHash;
    }

bool SimpleDocument::equals(Object::View v) const
    {
    SimpleDocument::View vThat = cast<SimpleDocument::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }
    else if (!super::equals(v))
        {
        return false;
        }

    return Object::equals(getDtdUri(), vThat->getDtdUri())     &&
           Object::equals(getDtdName(), vThat->getDtdName())   &&
           Object::equals(getEncoding(), vThat->getEncoding()) &&
           Object::equals(getDocumentComment(), vThat->getDocumentComment());
    }

COH_CLOSE_NAMESPACE3
