/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONVERTER_HPP
#define COH_CONVERTER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Interface for conversion from one {@link coherence::lang::Object
* Object} type to another one.
*
* @author mf  2007.07.05
*/
class COH_EXPORT Converter
    : public interface_spec<Converter>
    {
    // ----- Converter interface --------------------------------------------

    public:
        /**
        * Convert an {@link coherence::lang::Object Object} referenced
        * by the supplied holder.
        *
        * @param oh the holder to the Object to convert
        *
        * @return the holder to the converted Object
        */
        virtual Object::Holder convert(Object::Holder oh) const = 0;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Convert the {@link coherence::lang::Object Object} referenced
        * by the supplied Object::Holder using the supplied Converter.
        *
        * @param vConvert a handle to the Converter to use
        * @param oh       a holder to the Object to convert
        *
        * @return a holder of the converted Object, or the unconverted Object
        *         if the Converter is NULL
        */
        static Object::Holder convert(Converter::View vConvert, Object::Holder oh);

        /**
        * Convert the {@link coherence::lang::Object Object} referenced
        * by the supplied handle using the supplied Converter.
        *
        * @param vConvert a handle to the Converter to use
        * @param h        a handle to the Object to convert
        *
        * @return a handle of the converted Object, or the unconverted Object
        *         if the Converter is NULL
        */
        static Object::Handle convertHandle(Converter::View vConvert, Object::Handle h);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONVERTER_HPP
