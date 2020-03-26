/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_VALUE_HPP
#define COH_XML_VALUE_HPP

#include "coherence/lang.ns"

#include <ostream>

COH_OPEN_NAMESPACE3(coherence,run,xml)

class XmlElement;


/**
* An interface for XML element content and element attribute values.
*
* @author tb  2007.12.12
*/
class COH_EXPORT XmlValue
    : public interface_spec<XmlValue>
    {
    // ----- XmlValue interface ---------------------------------------------

    public:
        /**
        * Get the value as a bool.  If the internal value cannot be
        * translated into a bool, the supplied default value is
        * returned.
        *
        * @param fDefault  the default return value
        *
        * @return the value as a bool or the default value
        */
        virtual bool getBoolean(bool fDefault = false) const = 0;

        /**
        * Set the value as a bool.
        *
        * @param fValue  a new value of type bool
        */
        virtual void setBoolean(bool fValue) = 0;

        /**
        * Get the value as an integer.  If the internal value cannot be
        * translated into int32_t, the supplied default value is
        * returned.
        *
        * @param nDefault  the default return value
        *
        * @return the value as int32_t or the default value
        */
        virtual int32_t getInt32(int32_t nDefault = 0) const = 0;

        /**
        * Set the integer value.
        *
        * @param nValue  a new value of type int32_t
        */
        virtual void setInt32(int32_t nValue) = 0;

        /**
        * Get the value as a String.  If the internal value cannot be
        * translated into a String, the supplied default value is
        * returned.
        *
        * @param vsDefault  the default return value
        *
        * @return the value as a String or the default value
        */
        virtual String::View getString(String::View vsDefault = "") const = 0;

        /**
        * Set the String value.
        *
        * @param vsValue  a new value of type String
        */
        virtual void setString(String::View vsValue) = 0;

        /**
        * Get the value as an Object.  The following types are
        * supported:
        *
        * <ul>
        * <li>{@link coherence::lang::Boolean}</li>
        * <li>{@link coherence::lang::Integer32}</li>
        * <li>{@link coherence::lang::String}</li>
        * </ul>
        *
        * It is always legal for an implementation to return the value as a
        * String.
        *
        * @return the value as an Object or NULL if the XmlValue does
        *         not have a value; attributes never have a NULL value
        */
        virtual Object::View getValue() const = 0;

        /**
        * Get the parent element of this value.
        *
        * @return the parent element, or NULL if this value
        *         has no parent
        */
        virtual TypedHandle<XmlElement> getParent() = 0;

        /**
        * Get the parent element of this value as a view.
        *
        * @return the parent element, or NULL if this value
        *         has no parent
        */
        virtual TypedHandle<const XmlElement> getParent() const = 0;

        /**
        * Set the parent element of this value. Once set, the parent
        * cannot be reset.
        *
        * @param hElement  the parent element
        *
        * @throws coherence::lang::IllegalArgumentException
        *         if the specified parent is NULL
        * @throws coherence::lang::IllegalStateException
        *         if the parent is already set
        */
        virtual void setParent(TypedHandle<XmlElement> hElement) = 0;

        /**
        * Determine if the value is empty.
        *
        * @return true if the value is empty, false otherwise
        */
        virtual bool isEmpty() const = 0;

        /**
        * Determine if this value is an element attribute.
        *
        * @return true if this value is an element attribute, false
        *         otherwise
        */
        virtual bool isAttribute() const = 0;

        /**
        * Determine if this value is an element's content.
        *
        * @return true if this value is an element's content, false
        *         otherwise
        */
        virtual bool isContent() const = 0;

        /**
        * Format the value as XML.
        *
        * @return the XML formatted string
        */
        virtual String::View formatValue() const = 0;

        /**
        * Write the value as XML.
        *
        * @param out  an std::ostream to write to
        */
        COH_INLINE void writeValue(std::ostream& out) const
            {
            out << formatValue();
            }

    // ----- enumerated types useful for implementating this interface ------

    public:
        static const int32_t type_boolean    = 1;
        static const int32_t type_int        = 2;
        static const int32_t type_long       = 3;
        static const int32_t type_double     = 4;
        static const int32_t type_decimal    = 5;
        static const int32_t type_string     = 6;
        static const int32_t type_binary     = 7;
        static const int32_t type_date       = 8;
        static const int32_t type_time       = 9;
        static const int32_t type_datetime   = 10;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_VALUE_HPP
