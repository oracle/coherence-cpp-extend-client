/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/URI.hpp"

#include "private/coherence/net/URISyntaxException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


// ----- constructors/destructor---------------------------------------------

URI::URI(String::View vsUri)
        : f_vsScheme(self()),
          f_vsSSP(self()),
          f_vsFragment(self()),
          m_nHash(0)
    {
    if (NULL == vsUri || 0 == vsUri->length() || !vsUri->isASCII())
        {
        COH_THROW (URISyntaxException::create());
        }

    // extract optional scheme
    size32_t iSSP = vsUri->indexOf(':');
    if (String::npos == iSSP)
        {
        String::View vsNull;
        initialize(f_vsScheme, vsNull);
        }
    else if (iSSP == 0)
        {
        COH_THROW (URISyntaxException::create("zero-length scheme"));
        }
    else if (String::npos == vsUri->indexOf(':', iSSP + 1))
        {
        initialize(f_vsScheme, vsUri->substring(0, iSSP));
        }
    else
        {
        COH_THROW (URISyntaxException::create("multiple schemes"));
        }

    // extract required scheme-specific part
    size32_t iFragment = vsUri->indexOf('#', iSSP + 1);
    if (iSSP == iFragment || iSSP + 1 == iFragment)
        {
        COH_THROW (URISyntaxException::create(
                "missing required scheme-specific-part"));
        }
    initialize(f_vsSSP, vsUri->substring(iSSP + 1, iFragment));

    // extract optional fragment
    if (String::npos == iFragment)
        {
        String::View vsNull;
        initialize(f_vsFragment, vsNull);
        }
    else if (iFragment == vsUri->length() - 1)
        {
        COH_THROW (URISyntaxException::create("zero-length fragment"));
        }
    else if (String::npos == vsUri->indexOf('#', iFragment + 1))
        {
        initialize(f_vsFragment, vsUri->substring(iFragment + 1));
        }
    else
        {
        COH_THROW (URISyntaxException::create("multiple fragments"));
        }
    }

URI::URI(String::View vsScheme, String::View vsSSP, String::View vsFragment)
        : f_vsScheme(self(), vsScheme),
          f_vsSSP(self(), vsSSP),
          f_vsFragment(self(), vsFragment),
          m_nHash(0)
    {
    if (NULL == vsSSP || 0 == vsSSP->length())
        {
        COH_THROW (URISyntaxException::create(
                "missing required scheme-specific-part"));
        }
    }


// ----- URI interface ------------------------------------------------------

String::View URI::getScheme() const
    {
    return f_vsScheme;
    }

String::View URI::getSchemeSpecificPart() const
    {
    return f_vsSSP;
    }

String::View URI::getFragment() const
    {
    return f_vsFragment;
    }


// ----- Object interface ---------------------------------------------------

bool URI::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    URI::View vThat = cast<URI::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return Object::equals(this->f_vsScheme,   vThat->f_vsScheme) &&
           Object::equals(this->f_vsSSP,      vThat->f_vsSSP)    &&
           Object::equals(this->f_vsFragment, vThat->f_vsFragment);
    }

size32_t URI::hashCode() const
    {
    size32_t nHash = m_nHash;
    if (0 == nHash)
        {
        String::View vs = f_vsScheme;
        if (NULL != vs)
            {
            nHash = vs->hashCode();
            }

        nHash = 127 * nHash + f_vsSSP->hashCode();

        vs = f_vsFragment;
        if (NULL != vs)
            {
            nHash = 127 * nHash + vs->hashCode();
            }

        m_nHash = nHash;
        }

    return nHash;
    }

TypedHandle<const String> URI::toString() const
    {
    String::View vs;
    String::View vsScheme = f_vsScheme;
    if (NULL != vsScheme && 0 != vsScheme->length())
        {
        vs = COH_TO_STRING(vsScheme << ':' << f_vsSSP);
        }
    else
        {
        vs = COH_TO_STRING(f_vsSSP);
        }

    String::View vsFragment = f_vsFragment;
    if (NULL != vsFragment && 0 != vsFragment->length())
        {
        vs = COH_TO_STRING(vs << '#' << vsFragment);
        }

    return vs;
    }

COH_CLOSE_NAMESPACE2
