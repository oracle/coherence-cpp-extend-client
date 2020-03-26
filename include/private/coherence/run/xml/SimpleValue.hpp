/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_VALUE_HPP
#define COH_SIMPLE_VALUE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/run/xml/XmlValue.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* A simple implementation of the XmlValue interface.
*
* @author tb  2007.12.12
*/
class COH_EXPORT SimpleValue
    : public cloneable_spec<SimpleValue,
        extends<Object>,
        implements<XmlValue, PortableObject> >
    {
    friend class factory<SimpleValue>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SimpleValue instance with an empty value.
        */
        SimpleValue();

        /**
        * Create a new SimpleValue instance with a String value.
        *
        * @param vsValue     the value as String
        * @param fAttribute  true if the value is an attribute; false
        *                    otherwise
        */
        SimpleValue(String::View vsValue, bool fAttribute = false);

        /**
        * Create a new SimpleValue instance with an integer value.
        *
        * @param nValue      the value as int32_t
        * @param fAttribute  true if the value is an attribute; false
        *                    otherwise
        */
        SimpleValue(int32_t nValue, bool fAttribute = false);

        /**
        * Create a new SimpleValue instance with a boolean value.
        *
        * @param fValue      the value as bool
        * @param fAttribute  true if the value is an attribute; false
        *                    otherwise
        */
        SimpleValue(bool fValue, bool fAttribute = false);

        /**
        * Copy constructor.
        */
        SimpleValue(const SimpleValue& that);


    // ----- XmlValue interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool getBoolean(bool fDefault = false) const;

        /**
        * {@inheritDoc}
        */
        virtual void setBoolean(bool fValue);

        /**
        * {@inheritDoc}
        */
        virtual int32_t getInt32(int32_t nDefault = 0) const;

        /**
        * {@inheritDoc}
        */
        virtual void setInt32(int32_t nValue);

        /**
        * {@inheritDoc}
        */
        virtual String::View getString(String::View vsDefault = "") const;

        /**
        * {@inheritDoc}
        */
        virtual void setString(String::View vsValue);

        /**
        * {@inheritDoc}
        */
        virtual Object::View getValue() const;

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
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isAttribute() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isContent() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View formatValue() const;


  // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

    // ----- inner class: ValueType -----------------------------------------

    public:
        /**
        * Represents a value and knows how to convert between value
        * types.
        */
        class COH_EXPORT ValueType
            : public interface_spec<ValueType>
            {
            // ----- ValueType interface --------------------------------

            public:
                /**
                * Obtain the value as an integer. If unable to convert to an
                * int32_t, the supplied default value is returned.
                *
                * @param nDefault  the default value
                *
                * @return the value as an int32_t or the supplied default
                *         value
                */
                virtual int32_t getInt32Value(int32_t nDefault) const = 0;

                /**
                * Obtain the value as a boolean.  If unable to convert to a
                * boolean, the supplied default value is returned.
                *
                * @param fDefault  the default value
                *
                * @return the value as a bool or the supplied default
                *         value
                */
                virtual bool getBooleanValue(bool fDefault) const = 0;

                /**
                * Obtain the value as a String. If unable to convert to a
                * String, the supplied default value is returned.
                *
                * @param vsDefault  the default value
                *
                * @return the value as a String or the supplied default value
                */
                virtual String::View getStringValue(
                        String::View vsDefault) const = 0;

                /**
                * Obtain the value as an Object. For the integral types, the
                * corresponding Object type is returned. A bool is
                * returned as Boolean and int32_t is returned as an Integer32.
                *
                * @return the value as an Object
                */
                virtual Object::View getValue() const = 0;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Weak reference to the parent of this value.
        */
        WeakHandle<XmlElement> m_whParent;

        /**
        * True iff the value is an attribute.
        */
        bool m_fAttribute;

        /**
        * The underlying value object or NULL.
        */
        MemberView<ValueType> m_vValue;


    // ----- constants ------------------------------------------------------

    public:
        static const int32_t type_boolean  = 1;
        static const int32_t type_int32    = 2;
        static const int32_t type_int64    = 3;
        static const int32_t type_float64  = 4;
        static const int32_t type_float128 = 5;
        static const int32_t type_string   = 6;
        static const int32_t type_binary   = 7;
        static const int32_t type_date     = 8;
        static const int32_t type_time     = 9;
        static const int32_t type_datetime = 10;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_VALUE_HPP
