/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Converter.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- Converter interface : static methods -------------------------------

Object::Holder Converter::convert(Converter::View vConvert, Object::Holder oh)
    {
    return NULL == vConvert ? oh : vConvert->convert(oh);
    }

Object::Handle Converter::convertHandle(Converter::View vConvert, Object::Handle h)
    {
    return NULL == vConvert ? h : cast<Object::Handle>(vConvert->convert(h));
    }

COH_CLOSE_NAMESPACE2
