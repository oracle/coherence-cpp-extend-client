/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/annotation/PortableProperty.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,annotation)

COH_REGISTER_TYPED_CLASS(PortableProperty);

// ----- constructors -------------------------------------------------------

PortableProperty::PortableProperty(int32_t nIndex, Class::View vCodec)
    : m_nIndex(nIndex), f_vClzCodec(self(), vCodec)
    {
    }

// ----- Annotation members -------------------------------------------------

Class::View PortableProperty::annotationType() const
    {
    return SystemClassLoader::getInstance()->loadByType(typeid(PortableProperty));
    }

// ----- PortableProperty interface -----------------------------------------

int32_t PortableProperty::getIndex() const
    {
    return m_nIndex;
    }

Class::View PortableProperty::getCodec() const
    {
    return f_vClzCodec;
    }

Class::View PortableProperty::getStaticClass()
    {
    static FinalView<Class> vClzPortableProperty(System::common(),
            SystemClassLoader::getInstance()->loadByType(typeid(PortableProperty)));
    return vClzPortableProperty;
    }
COH_STATIC_INIT(PortableProperty::getStaticClass());

COH_CLOSE_NAMESPACE4
