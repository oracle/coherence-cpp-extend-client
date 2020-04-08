/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_SERIALIZATION_CONVERTER_HPP
#define COH_ABSTRACT_SERIALIZATION_CONVERTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Converter.hpp"

#include "coherence/io/Serializer.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::io::Serializer;

/**
* An abstract base for Serializer-based Converter implementations.
*
* @author nsa 2008.05.29
*/
class COH_EXPORT AbstractSerializationConverter
    : public abstract_spec<AbstractSerializationConverter,
        extends<Object>,
        implements<coherence::util::Converter> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractSerializationConverter();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractSerializationConverter(const AbstractSerializationConverter&);
		

    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Serializer used by this Converter.
        *
        * @return the Serializer used by this Converter
        */
        virtual Serializer::View getSerializer() const;

        /**
        * Set the Serializer to be used by this Converter.
        *
        * @param vSerializer  the Serializer to be used by this Converter
        */
        virtual void setSerializer(Serializer::View vSerializer);

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A Serializer used by this Converter.
        */
        FinalView<Serializer> f_vSerializer;
    };

COH_CLOSE_NAMESPACE3

#endif /*COH_ABSTRACT_SERIALIZATION_CONVERTER_HPP*/
