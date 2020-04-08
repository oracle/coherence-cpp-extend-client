/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/reflect/Codecs.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

COH_REGISTER_CLASS(TypedBarrenClass<Codec>::create());

// ----- factory methods ----------------------------------------------------

Codec::View Codecs::getCodec(Class::View vClz)
    {
    // class is assignable to Codec
    ClassLoader::View vLoader   = SystemClassLoader::getInstance();
    Class::View       vClzCodec = vLoader->loadByType(typeid(Codec));
    Codec::View       vCodec    = NULL;

    // plausibility of assignment performs NULL check
    if (vClzCodec->isAssignableFrom(vClz))
        {
        vCodec = cast<Codec::View>(vClz->newInstance());
        }
    else
        {
        vCodec = Codecs::getDefault();
        }

    return vCodec;
    }

// ----- AbstractCodec: Codec interface -------------------------------------

void Codecs::AbstractCodec::encode(PofWriter::Handle hOut, int32_t nIndex, Object::View voValue) const
    {
    hOut->writeObject(nIndex, voValue);
    }

// ----- ObjectCodec: Codec interface ---------------------------------------

Object::Holder Codecs::DefaultCodec::decode(PofReader::Handle hIn, int32_t nIndex) const
    {
    return hIn->readObject(nIndex);
    }

// ----- constants ----------------------------------------------------------

Codec::View Codecs::getDefault()
    {
    static FinalView<Codecs::DefaultCodec> vCodec(System::common(),
            Codecs::DefaultCodec::create());
    return vCodec;
    }
COH_STATIC_INIT(Codecs::getDefault());

COH_CLOSE_NAMESPACE4
