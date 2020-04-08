/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VIEW_ONLY_ELEMENT_HPP
#define COH_VIEW_ONLY_ELEMENT_HPP

#include "coherence/lang.ns"

#include "private/coherence/run/xml/SimpleElement.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


class COH_EXPORT ViewOnlyElement
    : public cloneable_spec<ViewOnlyElement,
        extends<SimpleElement> >
    {
    friend class factory<ViewOnlyElement>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SimpleElement with an optional name.
        *
        * @param vsName  name of the element, defaults to NULL
        */
        ViewOnlyElement(XmlElement::View vParent, String::View vsName);

        /**
        * Create a SimpleElement with a name and string value.
        *
        * @param vsName   name of the element
        * @param vsValue  value as a String
        */
        ViewOnlyElement(XmlElement::View vParent, String::View vsName, String::View vsValue);

        /**
        * Create a SimpleElement with a name and boolean value.
        *
        * @param vsName  name of the element
        * @param fValue  value as a bool
        */
        ViewOnlyElement(XmlElement::View vParent, String::View vsName, int32_t nValue);

        /**
        * Create a SimpleElement with a name and integer value.
        *
        * @param vsName  name of the element
        * @param nValue  value as an int32_t
        */
        ViewOnlyElement(XmlElement::View vParent, String::View vsName, bool fValue);

        /**
        * Copy Constructor.
        */
        ViewOnlyElement(const ViewOnlyElement& that);


    // ----- XmlElement interface -------------------------------------------

    public:

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<XmlElement> getParent();

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const XmlElement> getParent() const;

        /**
        * {@inheritDoc}
        */
        virtual void setParent(TypedHandle<XmlElement> hElement);

        /**
        * Set the parent as a view only object.
        */
        virtual void setParent(TypedHandle<const XmlElement> vElement);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Weak reference to the parent of this value.
        */
        WeakView<XmlElement> m_wvParent;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_VIEW_ONLY_ELEMENT_HPP
