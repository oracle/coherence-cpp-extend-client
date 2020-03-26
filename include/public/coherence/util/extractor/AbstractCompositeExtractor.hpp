/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_COMPOSITE_EXTRACTOR_HPP
#define COH_ABSTRACT_COMPOSITE_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Abstract super class for ValueExtractor implementations that are based on
* an underlying array of ValueExtractor objects.
*
* @author djl  2008.03.05
*/
class COH_EXPORT AbstractCompositeExtractor
    : public abstract_spec<AbstractCompositeExtractor,
        extends<AbstractExtractor> >
    {
    friend class factory<AbstractCompositeExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        AbstractCompositeExtractor();

        /**
        * Construct a AbstractCompositeExtractor based on the specified
        * ValueExtractor array.
        *
        * @param vaExtractor  the ValueExtractor array
        */
        AbstractCompositeExtractor(ObjectArray::View vaExtractor);

        /**
        * Construct a AbstractCompositeExtractor based on two ValueExtractors.
        *
        * @param vaE1  the first ValueExtractor
        * @param vaE2  the second ValueExtractor
        */
        AbstractCompositeExtractor(ValueExtractor::View vE1,
                ValueExtractor::View vE2);

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractCompositeExtractor(const AbstractCompositeExtractor&);


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
        * Compare the AbstractCompositeExtractor with another object to
        * determine equality. Two AbstractCompositeExtractor objects are
        * considered equal iff they belong to the same class and their
        * underlying ValueExtractor arrays are deep-equal.
        *
        * @return true iff this AbstractCompositeExtractor and the passed
        *         object are equivalent
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
        * Obtain the ValueExtractor array.
        *
        * @return the ValueExtractor array
        */
        virtual ObjectArray::View getExtractors() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The ValueExtractor array.
        */
        FinalView<ObjectArray> f_vaExtractor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_COMPOSITE_EXTRACTOR_HPP
