/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/SimpleValue.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/run/xml/XmlElement.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Binary.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"

#include <sstream>

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::io::IOException;
using coherence::io::pof::RawDate;
using coherence::io::pof::RawDateTime;
using coherence::io::pof::RawTime;
using coherence::util::Binary;

COH_REGISTER_PORTABLE_CLASS(502, SimpleValue);


// ----- local class: StringValue -------------------------------------------

/**
* String implementation of the ValueType interface.
*/
class StringValue
     : public cloneable_spec<StringValue,
         extends<Object>,
         implements<SimpleValue::ValueType> >
    {
    friend class factory<StringValue>;

    // ----- constructors -----------------------------------------------

    protected:
        StringValue(String::View vs)
            : f_vsValue(self(), vs)
            {
            }


        StringValue(const StringValue& that)
            : super(that), f_vsValue(self(), that.f_vsValue)
            {
            }

    // ----- ValueType interface ----------------------------------------

    public:
        virtual int32_t getInt32Value(int32_t nDefault) const
            {
            String::View vsValue = f_vsValue;
            if (NULL == vsValue)
                {
                return nDefault;
                }

            int32_t            nValue = 0;
            std::istringstream iss(vsValue->getCString());
            if ((iss >> std::dec >> nValue).fail())
                {
                return nDefault;
                }
            else
                {
                return nValue;
                }
            }

        virtual bool getBooleanValue(bool fDefault) const
            {
            String::View vsValue = XmlHelper::toUpperCase(f_vsValue);
            if (NULL == vsValue)
                {
                return fDefault;
                }

            if (vsValue->equals("T") || vsValue->equals("TRUE") ||
                vsValue->equals("Y") || vsValue->equals("YES"))
                {
                return true;
                }
            else if (vsValue->equals("F") || vsValue->equals("FALSE") ||
                     vsValue->equals("N") || vsValue->equals("NO"))
                {
                return false;
                }
            else
                {
                int32_t nValue = getInt32Value(-1);
                if (nValue == 0)
                    {
                    return false;
                    }
                else if (nValue == 1)
                    {
                    return true;
                    }
                return fDefault;
                }
            }

        virtual String::View getStringValue(String::View /*vsDefault*/) const
            {
            return f_vsValue;
            }

        virtual Object::View getValue() const
            {
            return f_vsValue;
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * Internal value.
        */
        FinalView<String> f_vsValue;
    };


// ----- local class: IntValue ----------------------------------------------

/**
* Int implementation of the ValueType interface.
*/
class IntValue
     : public cloneable_spec<IntValue,
         extends<Object>,
         implements<SimpleValue::ValueType> >
    {
    friend class factory<IntValue>;

    // ----- constructors -----------------------------------------------

    protected:
        IntValue(int32_t nValue)
            : m_nValue(nValue)
            {
            }

        IntValue(const IntValue& that)
            : super(that), m_nValue(that.m_nValue)
            {
            }

    // ----- ValueType interface ----------------------------------------

    public:
        virtual int32_t getInt32Value(int32_t /*nDefault*/) const
            {
            return m_nValue;
            }

        virtual bool getBooleanValue(bool fDefault) const
            {
            if (0 == m_nValue)
                {
                return false;
                }
            else if (1 == m_nValue)
                {
                return true;
                }
            else
                {
                return fDefault;
                }
            }

        virtual String::View getStringValue(String::View /*vsDefault*/) const
            {
            return COH_TO_STRING(m_nValue);
            }

        virtual Object::View getValue() const
            {
            return Integer32::valueOf(m_nValue);
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * Internal value.
        */
        int32_t m_nValue;
    };


// ----- local class: BoolValue ---------------------------------------------

/**
* Bool implementation of the ValueType interface.
*/
class BoolValue
     : public cloneable_spec<BoolValue,
         extends<Object>,
         implements<SimpleValue::ValueType> >
    {
    friend class factory<BoolValue>;

    // ----- constructors -----------------------------------------------

    protected:
        BoolValue(bool fValue)
            : m_fValue(fValue)
            {
            }

        BoolValue(const BoolValue& that)
            : super(that), m_fValue(that.m_fValue)
            {
            }

    // ----- ValueType interface ----------------------------------------

    public:
        virtual int32_t getInt32Value(int32_t /*nDefault*/) const
            {
            return m_fValue;
            }

        virtual bool getBooleanValue(bool /*fDefault*/) const
            {
            return m_fValue;
            }

        virtual String::View getStringValue(String::View /*vsDefault*/) const
            {
            return m_fValue ? "true" : "false";
            }

        virtual Object::View getValue() const
            {
            return Boolean::valueOf(m_fValue);
            }

   // ----- data members -----------------------------------------------

    protected:
        /**
        * Internal value.
        */
        bool m_fValue;
    };


// ----- constructors/destructor  -------------------------------------------

SimpleValue::SimpleValue()
        : m_whParent(self()), m_fAttribute(false), m_vValue(self())
    {
    }

SimpleValue::SimpleValue(String::View vsValue, bool fAttribute)
        : m_whParent(self()), m_fAttribute(fAttribute),
          m_vValue(self(), StringValue::create(vsValue))
    {
    }

SimpleValue::SimpleValue(int32_t nValue, bool fAttribute)
        : m_whParent(self()), m_fAttribute(fAttribute),
          m_vValue(self(), IntValue::create(nValue))
    {
    }

SimpleValue::SimpleValue(bool fValue, bool fAttribute)
    : m_whParent(self()), m_fAttribute(fAttribute),
      m_vValue(self(), BoolValue::create(fValue))
    {
    }

SimpleValue::SimpleValue(const SimpleValue& that)
    : super(that),
      m_whParent(self()),
      m_fAttribute(false),
      m_vValue(self(), StringValue::create(that.getString()))
    {
    }


// ----- XmlValue interface -------------------------------------------------

void SimpleValue::setBoolean(bool fValue)
    {
    m_vValue = BoolValue::create(fValue);
    }

void SimpleValue::setInt32(int32_t nValue)
    {
    m_vValue = IntValue::create(nValue);
    }

void SimpleValue::setString(String::View vsValue)
    {
    m_vValue = StringValue::create(vsValue);
    }

bool SimpleValue::getBoolean(bool fDefault) const
    {
    ValueType::View vValue = m_vValue;
    return NULL == vValue ? fDefault : vValue->getBooleanValue(fDefault);
    }

int32_t SimpleValue::getInt32(int32_t nDefault) const
    {
    ValueType::View vValue = m_vValue;
    return NULL == vValue ? nDefault : vValue->getInt32Value(nDefault);
    }

String::View SimpleValue::getString(String::View vsDefault) const
    {
    ValueType::View vValue = m_vValue;
    return NULL == vValue ? vsDefault : vValue->getStringValue(vsDefault);
    }

Object::View SimpleValue::getValue() const
    {
    ValueType::View vValue = m_vValue;
    return NULL == vValue ? (Object::View) NULL : vValue->getValue();
    }

TypedHandle<XmlElement> SimpleValue::getParent()
    {
    return m_whParent;
    }

TypedHandle<const XmlElement> SimpleValue::getParent() const
    {
    return m_whParent;
    }

void SimpleValue::setParent(TypedHandle<XmlElement> hElement)
    {
    COH_ENSURE_PARAM(hElement);
    COH_ENSURE(m_whParent == NULL);

    m_whParent = hElement;
    }

bool SimpleValue::isEmpty() const
    {
    Object::View o = m_vValue;

    if (is_null(o))
        {
        return true;
        }

    if (instanceof<StringValue::View>(o) &&
            (cast<StringValue::View>(m_vValue))->getStringValue("")->length() == 0)
        {
        return true;
        }

    return false;
    }

bool SimpleValue::isAttribute() const
    {
    return m_fAttribute;
    }

bool SimpleValue::isContent() const
    {
    return !isAttribute();
    }

String::View SimpleValue::formatValue() const
    {
    String::View vs = getString();
    return COH_TO_STRING((isAttribute() ?
            XmlHelper::quote(vs) :
            XmlHelper::encodeContent(vs, true)));
    }


// ----- PortableObject interface -------------------------------------------

void SimpleValue::readExternal(PofReader::Handle hIn)
    {
    if (m_vValue != NULL || m_fAttribute)
        {
        COH_THROW (IllegalArgumentException::create(""));
        }

    switch (hIn->readOctet(0))
        {
        case 0:
                break;

        case type_boolean:
                m_vValue = cast<ValueType::View>(hIn->readBoolean(1)
                    ? Boolean::create(true) : Boolean::create(false));
                break;

        case type_int32:
                m_vValue = cast<ValueType::View>(Integer32::valueOf(hIn->readInt32(1)));
                break;

        case type_int64:
                m_vValue = cast<ValueType::View>(Integer64::valueOf(hIn->readInt64(1)));
                break;

        case type_float64:
                m_vValue = cast<ValueType::View>(Float64::valueOf(hIn->readFloat64(1)));
                break;

                 //TODO: implement readBigDecimal
                //case TYPE_DECIMAL:
                //m_vValue = Float128::valueOf(in->readFloat128(1));
                //break;

        case type_string:
            m_vValue = cast<ValueType::View>(hIn->readString(1));
                break;

        case type_binary:
            m_vValue = cast<ValueType::View>(hIn->readBinary(1));
                break;

        case type_date:
             m_vValue = cast<ValueType::View>(hIn->readRawDate(1));
                 break;

        case type_time:
                 m_vValue = cast<ValueType::View>(hIn->readRawTime(1));
                 break;

        case type_datetime:
                 m_vValue = cast<ValueType::View>(hIn->readRawDateTime(1));
                 break;

         default:
             COH_THROW (IOException::create(""));
        }

    m_fAttribute = hIn->readBoolean(2);
    }

void SimpleValue::writeExternal(PofWriter::Handle hOut) const
    {
    Object::View o = m_vValue;
    if (o == NULL)
        {
        hOut->writeOctet(0, 0);
        }
    else if (instanceof<String::View>(o))
        {
        hOut->writeOctet(0, type_string);
        hOut->writeString(1, cast<String::View>(o));
        }
    else if (instanceof<Boolean::View>(o))
        {
        hOut->writeOctet(0, type_boolean);
        hOut->writeBoolean(1, (cast<Boolean::View>(o))->getValue());
        }
    else if (instanceof<Integer32::View>(o))
        {
        hOut->writeOctet(0, type_int32);
        hOut->writeInt32(1, (cast<Integer32::View>(o))->getValue());
        }
    else if (instanceof<Integer64::View>(o))
        {
        hOut->writeOctet(0, type_int64);
        hOut->writeInt64(1, (cast<Integer64::View>(o))->getValue());
        }
    else if (instanceof<Float64::View>(o))
        {
        hOut->writeOctet(0, type_float64);
        hOut->writeFloat64(1, (cast<Float64::View>(o))->getValue());
        }
    // else if (instanceof<Float128::View>(o))
      // {
      // hOut.writeByte(0, type_float128);
      // hOut.writeFloat128(1, (cast<Float128::View>(o))->getValue());
      // }
    else if (instanceof<Binary::View>(o))
        {
        hOut->writeOctet(0, type_binary);
        hOut->writeBinary(1, cast<Binary::View>(o));
        }
    else if (instanceof<RawDate::View>(o))
        {
        hOut->writeOctet(0, type_date);
        hOut->writeRawDate(1, cast<RawDate::View>(o));
        }
    else if (instanceof<RawTime::View>(o))
        {
        hOut->writeOctet(0, type_time);
        hOut->writeRawTime(1, cast<RawTime::View>(o));
        }
    else if (instanceof<RawDateTime::View>(o))
        {
        hOut->writeOctet(0, type_datetime);
        hOut->writeRawDateTime(1, cast<RawDateTime::View>(o));
        }
    else
        {
        COH_THROW (IOException::create("unsupported type to write"));
        }

    hOut->writeBoolean(2, m_fAttribute);
    }


// ----- Object interface ---------------------------------------------------

bool SimpleValue::equals(Object::View v) const
    {
    XmlValue::View vThat = cast<XmlValue::View>(v, false);

    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }
    else
        {
        return Object::equals(getString(), vThat->getString());
        }
    }

size32_t SimpleValue::hashCode() const
    {
    return getString()->hashCode();
    }

TypedHandle<const String> SimpleValue::toString() const
    {
    return formatValue();
    }

COH_CLOSE_NAMESPACE3
