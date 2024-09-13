/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNIVERSAL_EXTRACTOR_HPP
#define COH_UNIVERSAL_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

/**
 * Universal ValueExtractor implementation.
 *
 * UniversalExtractor can only run within the Coherence cluster.
 *
 * Refer to the Coherence for Java documentation for more information.
 *
 * @author cp/gg 2002.11.01, ew 2007.02.01, jf 2017.11.20, phf 2024.09.13
 *
 * @since 14.1.2.0.0
 */
class COH_EXPORT UniversalExtractor
    : public cloneable_spec<UniversalExtractor,
        extends<AbstractExtractor> >
    {
    friend class factory<UniversalExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct an empty UniversalExtractor
         * (necessary for the PortableObject interface).
         */
        UniversalExtractor();

        /**
         * Construct a UniversalExtractor based on a name, optional
         * parameters and the entry extraction target.
         *
         * @param vsName   a method or property name
         * @param vaParam  the array of arguments to be used in the method
         *                 invocation; may be null
         * @param nTarget  one of the {@link #value} or {@link #key} values
         */
        UniversalExtractor(String::View vsName, ObjectArray::View vaParam = NULL,
            int32_t nTarget = value);

        /**
         * Copy constructor.
         */
        UniversalExtractor(const UniversalExtractor& that);

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

    // ----- data member accessors ------------------------------------------

    public:
        /**
         * Return the name passed into UniversalExtractor(String).
         *
         * @return the name of extraction attribute
        */
        virtual String::View getName() const;

        /**
        * Return the array of arguments used to invoke the method.
        *
        * @return the array of arguments used to invoke the method
        */
        virtual ObjectArray::View getParameters() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
         * A method or property name.
         */
        FinalView<String> f_vsName;

        /**
         * The parameter array. Must be null or zero length for a property based extractor.
         */
        FinalView<ObjectArray> f_vaParam;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_UNIVERSAL_EXTRACTOR_HPP
