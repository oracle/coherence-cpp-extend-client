/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPARISON_EXTRACTOR_HPP
#define COH_COMPARISON_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/extractor/AbstractCompositeExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* A synthetic ValueExtractor that returns a result of comparison between two
* values extracted from the same target. In a most general case, the extracted
* value represents an Integer value calculated accordingly to the contract of
* Comparable#compareTo or Comparator#compare methods. However, in more
* specific cases, when the compared values are of common numeric type, the
* ComparisonValueExtractor will return a numeric difference between those
* values. The type of the comparing values will dictate the type of
* the result.
*
* For example, lets assume that a cache contains business objects that have two
* properties: SellPrice and BuyPrice (both double). Then, to query for all
* objects that have SellPrice less than BuyPrice we would use the following:
* <pre>
* ValueExtractor::View extractDiff = ComparisonValueExtractor::create(
*   ReflectionExtractor::create(String::create("getSellPrice")),
*   ReflectionExtractor::create(String::create("getBuyPrice")));
* Filter::View vfilter = LessFilter::create(extractDiff, Double::create(0.0));
* Set::View entries = cache->entrySet(vfilter);
* </pre>
*
* @author djl 2008.04.11
*
* @see ChainedExtractor
*/
class COH_EXPORT ComparisonValueExtractor
    : public class_spec<ComparisonValueExtractor,
        extends<AbstractCompositeExtractor> >
    {
    friend class factory<ComparisonValueExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an empty ComparisonValueExtractor
        * (necessary for the PortableObject interface).
        */
        ComparisonValueExtractor();

        /**
        * Construct a ComparisonValueExtractor based on two specified extractors and
        * a Comparator object.
        *
        * @param vE1    the ValueExtractor for the first value
        * @param vE2    the ValueExtractor for the second value
        * @param vComp  the comparator used to compare the extracted values
        *               (optional); if NULL, the values returned by both
        *               extractors must be Comparable
        */
        ComparisonValueExtractor(ValueExtractor::View vE1,
                ValueExtractor::View vE2,
                Comparator::View vComp = NULL);


   // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException always
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


    // ----- data member accessors ------------------------------------------

    public:
       /**
      * Return a Comparator used by this extractor.
      *
      * @return a Comparator used by this extractor; null if the natural value
      *         comparison should be used
      */
        virtual Comparator::View getComparator() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * An underlying Comparator object (optional).
        */
       FinalView<Comparator> f_vComparator;
    };

COH_CLOSE_NAMESPACE3

#endif // #ifndef COH_COMPARISON_EXTRACTOR_HPP
