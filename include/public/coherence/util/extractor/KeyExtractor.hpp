/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_KEY_EXTRACTOR_HPP
#define COH_KEY_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* The KeyExtractor is a special purpose ValueExtractor implementation that
* serves as an indicator that a query should be run against the key objects
* rather than the values. The major difference between the KeyExtractor and a
* standard ReflectionExtractor is that when used in various EntryFilter
* implementations it forces the evaluation of entry keys rather than entry
* values.
*
* For example, consider a key object that consists of two properties:
* "FirstName" and "LastName". To retrieve all keys that have a value of the
* "LastName" property equal to "Smith", the following query could be used:
* <pre>
* ValueExtractor::View extractor = KeyExtractor::create("getLastName");
* Set::View setKeys = cache->keySet(EqualsFilter::create(extractor, "Smith"));
*
* </pre>
* As of Coherence 3.5, the same effect can be achieved for subclasses of the
* AbstractExtractor, for example:
* <pre>
* ValueExtractor::View vExtractor = ReflectionExtractor::create("getLastName",
*       NULL, AbstractExtractor::KEY);
* Set::View setKeys = cache->keySet(EqualsFilter::create(vExtractor, "Smith"));
* </pre>
*
* @author djl 2008.03.24
*/
class COH_EXPORT KeyExtractor
    : public class_spec<KeyExtractor,
        extends<AbstractExtractor> >
    {
    friend class factory<KeyExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create and return a Handle to a new Object instance.
        */
        KeyExtractor();

        /**
        * Construct a KeyExtractor based on a specified ValueExtractor.
        *
        * @param vExtractor  the underlying ValueExtractor
        */
        KeyExtractor(ValueExtractor::View vExtractor);


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


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the underlying ValueExtractor.
        *
        * @return the ValueExtractor
        */
        virtual ValueExtractor::View getExtractor() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying ValueExtractor.
        */
        MemberView<ValueExtractor> m_vExtractor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_IDENTITY_EXTRACTOR_HPP
