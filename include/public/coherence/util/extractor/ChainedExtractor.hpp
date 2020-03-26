/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CHAINED_EXTRACTOR_HPP
#define COH_CHAINED_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/extractor/AbstractCompositeExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* Composite ValueExtractor implementation based on an array of extractors.
* The extractors in the array are applied sequentially left-to-right, so a
* result of a previous extractor serves as a target object for a next one.
*
* @author djl 2008.03.07
*
* @see ChainedExtractor
*/
class COH_EXPORT ChainedExtractor
    : public class_spec<ChainedExtractor,
        extends<AbstractCompositeExtractor> >
    {
    friend class factory<ChainedExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ChainedExtractor();

        /**
        * Return a ChainedExtractor based on a specified ValueExtractor array.
        *
        * @param vaExtractor  the ValueExtractor array
        */
        ChainedExtractor(ObjectArray::View vaExtractor);

        /**
        * Construct a ChainedExtractor based on two extractors.
        *
        * @param vExtractor1  the ValueExtractor
        * @param vExtractor2  the ValueExtractor
        */
        ChainedExtractor(ValueExtractor::View vExtractor1,
                ValueExtractor::View vExtractor2);

    private:
        /**
        * Blocked copy constructor.
        */
        ChainedExtractor(const ChainedExtractor&);


    // ----- AbstractExtractor methods --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const;

        /**
        * Extract the value from the passed entry. The underlying extractors
        * are applied sequentially, so a result of a previous extractor serves
        * as a target object for a next one. A value of null prevents any
        * further extractions and is returned immediately. For intrinsic types,
        * the returned value is expected to be a standard wrapper type in the
        * same manner that reflection works; for example, int would be
        * returned as a Integer32.
        */
        virtual Object::Holder extractFromEntry(Map::Entry::Holder ohEntry) const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Parse a dot-delimited sequence of method names and instantiate
        * a corresponding array of {@link ReflectionExtractor} objects.
        *
        * @param vsName  a dot-delimited sequence of method names
        *
        * @return an array of {@link ReflectionExtractor} objects
        */
        static ObjectArray::View createExtractors(String::View vsName);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CHAINED_EXTRACTOR_HPP
