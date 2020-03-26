/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/ViewOnlyElement.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)



// ----- constructors --------------------------------------------------------

ViewOnlyElement::ViewOnlyElement(XmlElement::View vParent, String::View vsName)
    : super(vsName), m_wvParent(self(), vParent)
    {
    }

ViewOnlyElement::ViewOnlyElement(XmlElement::View vParent,
        String::View vsName, String::View vsValue)
    : super(vsName, vsValue), m_wvParent(self(), vParent)
    {
    }

ViewOnlyElement::ViewOnlyElement(XmlElement::View vParent, String::View vsName, int32_t nValue)
    : super(vsName, nValue), m_wvParent(self(), vParent)
    {
    }

ViewOnlyElement::ViewOnlyElement(XmlElement::View vParent, String::View vsName, bool fValue)
    : super(vsName, fValue), m_wvParent(self(), vParent)
    {
    }

ViewOnlyElement::ViewOnlyElement(const ViewOnlyElement& that)
    : super(that), m_wvParent(self(), that.m_wvParent)
    {
    }


// ----- XmlElement interface -----------------------------------------------

TypedHandle<XmlElement> ViewOnlyElement::getParent()
    {
    COH_THROW (IllegalStateException::create());
    }

TypedHandle<const XmlElement> ViewOnlyElement::getParent() const
    {
    return m_wvParent;
    }

void ViewOnlyElement::setParent(TypedHandle<XmlElement> hElement)
    {
    m_wvParent = hElement;
    }

void ViewOnlyElement::setParent(TypedHandle<const XmlElement> vElement)
    {
    m_wvParent = vElement;
    }

COH_CLOSE_NAMESPACE3
