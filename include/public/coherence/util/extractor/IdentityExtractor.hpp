/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IDENTITY_EXTRACTOR_HPP
#define COH_IDENTITY_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Trivial ValueExtractor implementation that does not actually extract
* anything from the passed value, but returns the value itself.
*
* @author djl  2008.03.05
*/
class COH_EXPORT IdentityExtractor
    : public class_spec<IdentityExtractor,
        extends<AbstractExtractor> >
    {
    friend class factory<IdentityExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create and return a Handle to a new Object instance.
        */
        IdentityExtractor();


    // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const;


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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * An instance of the IdentityExtractor.
        */
        static IdentityExtractor::Handle getInstance();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_IDENTITY_EXTRACTOR_HPP
