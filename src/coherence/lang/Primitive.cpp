/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Primitive.hpp"

#include "coherence/lang/Boolean.hpp"
#include "coherence/lang/Float32.hpp"
#include "coherence/lang/Float64.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::StringHelper;


// ----- constructors -------------------------------------------------------

template<class T> Primitive<T>::Primitive(const T& value)
    : m_value(value)
    {
    }

template<class T> Primitive<T>::Primitive(const Primitive& that)
    : Primitive<T>::super(that), m_value(that.m_value)
    {
    }


// ----- Primitive interface ------------------------------------------------

template<class T> T Primitive<T>::getValue() const
    {
    return m_value;
    }

template<class T> Primitive<T>::operator T() const
    {
    return m_value;
    }

template<class T> T Primitive<T>::parse(String::View vStr, size_t nPrecision)
    {
    std::stringstream s;
    T result;
    s << vStr;
    s >> result;

    if (NULL == vStr || s.fail() || !s.eof())
        {
        COH_THROW_STREAM(IllegalArgumentException, "\"" << vStr <<
                "\" is not valid.");
        }

    if (nPrecision > 0)
        {
        std::stringstream ss;
        ss << std::fixed << std::setprecision((int) nPrecision) << result;
        ss >> result;
        }

    return result;
    }

template<> bool Primitive<bool>::parse(String::View vStr, size_t /*nPrecision*/)
    {
    // parse booleans using the same rules as Java
    return NULL != vStr && StringHelper::compare(vStr, "true") == 0;
    }

template<> wchar16_t Primitive<wchar16_t>::parse(String::View vStr, size_t /*nPrecision*/)
    {
    if (NULL == vStr || vStr->isEmpty() || vStr->length() > 1)
        {
        COH_THROW_STREAM(IllegalArgumentException, "\"" << vStr <<
                "\" is not valid.");
        }
    const char *pch = vStr->getCString();
    return String::next(pch);
    }


// ----- Comparable interface -----------------------------------------------

template<class T> int32_t Primitive<T>::compareTo(Object::View v) const
    {
    T valueThis = getValue();
    T valueThat = cast<typename Primitive<T>::View>(v)->getValue();

    return valueThis < valueThat ? -1 : (valueThis == valueThat ? 0 : 1);
    }

template<> int32_t Primitive<float32_t>::compareTo(Object::View v) const
    {
    float32_t valueThis = getValue();
    float32_t valueThat = cast<Primitive<float32_t>::View>(v)->getValue();

    return Float32::compare(valueThis, valueThat);
    }

template<> int32_t Primitive<float64_t>::compareTo(Object::View v) const
    {
    float64_t valueThis = getValue();
    float64_t valueThat = cast<Primitive<float64_t>::View>(v)->getValue();

    return Float64::compare(valueThis, valueThat);
    }


// ----- Object interface ---------------------------------------------------

template<class T> bool Primitive<T>::equals(Object::View v) const
    {
    if (NULL == v)
        {
        return false;
        }

    if (this == v)
        {
        return true;
        }

    typename abstract_spec<Primitive<T>,
        extends<Object>, implements<Comparable> >::View vThat =
            cast<typename abstract_spec<Primitive<T>,
                extends<Object>, implements<Comparable> >::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return getValue() == vThat->getValue();
    }

template<> bool Primitive<float32_t>::equals(Object::View v) const
    {
    if (NULL == v)
        {
        return false;
        }

    if (this == v)
        {
        return true;
        }

    Float32::View vThat = cast<Float32::View>(v,false);
    if (NULL == vThat)
        {
        return false;
        }

    return Float32::compare(getValue(), vThat->getValue()) == 0;
    }

template<> bool Primitive<float64_t>::equals(Object::View v) const
    {
    if (NULL == v)
        {
        return false;
        }

    if (this == v)
        {
        return true;
        }

    Float64::View vThat = cast<Float64::View>(v,false);
    if (NULL == vThat)
        {
        return false;
        }

    return Float64::compare(getValue(), vThat->getValue()) == 0;

    }

template<> size32_t Primitive<bool>::hashCode() const
    {
    return getValue() ? 1231 : 1237;
    }

template<class T> size32_t Primitive<T>::hashCode() const
    {
    return (size32_t) getValue();
    }

template<class T> bool Primitive<T>::isImmutable() const
    {
    return true;
    }

template<class T> TypedHandle<const String> Primitive<T>::toString() const
    {
    return COH_TO_STRING(getValue());
    }

template<> TypedHandle<const String> Primitive<bool>::toString() const
    {
    return Boolean::toString(getValue());
    }


// ----- Primitive template instantiations ----------------------------------

template class COH_EXPORT coherence::lang::Primitive<bool>;
template class COH_EXPORT coherence::lang::Primitive<octet_t>;
template class COH_EXPORT coherence::lang::Primitive<wchar16_t>;
template class COH_EXPORT coherence::lang::Primitive<int16_t>;
template class COH_EXPORT coherence::lang::Primitive<int32_t>;
template class COH_EXPORT coherence::lang::Primitive<int64_t>;
template class COH_EXPORT coherence::lang::Primitive<size32_t>;
template class COH_EXPORT coherence::lang::Primitive<size64_t>;
template class COH_EXPORT coherence::lang::Primitive<float32_t>;
template class COH_EXPORT coherence::lang::Primitive<float64_t>;

COH_CLOSE_NAMESPACE2
