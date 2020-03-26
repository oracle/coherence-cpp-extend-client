/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MULTI_EXTRACTOR_HPP
#define COH_MULTI_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/QueryMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/AbstractCompositeExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


/**
* Composite ValueExtractor implementation based on an array of extractors.
* All extractors in the array are applied to the same target object and the
* result of the extraction is a List of extracted values.
*
* Common scenarios for using the MultiExtractor involve the DistinctValues or
* GroupAggregator aggregators, that allow clients to collect all distinct
* combinations of a given set of attributes or collect and run additional
* aggregation against the corresponding groups of entries.
*
* @author djl 2008.04.09
*/
class COH_EXPORT MultiExtractor
    : public class_spec<MultiExtractor,
        extends<AbstractCompositeExtractor> >
    {
    friend class factory<MultiExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        MultiExtractor();

        /**
        * Construct a MultiExtractor.
        *
        * @param vaExtractor  the ValueExtractor array
        */
        MultiExtractor(ObjectArray::View vaExtractor);

    private:
        /**
        * Blocked copy constructor.
        */
        MultiExtractor(const MultiExtractor&);


    // ----- AbstractExtractor methods --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(Object::Holder ohTarget) const;

        /**
        * Extract the value from the passed entry. The underlying extractors
        * are applied sequentially, so a result of a previous extractor serves
        * as a target object for a next one. A value of NULL prevents any
        * further extractions and is returned immediately. For intrinsic types,
        * the returned value is expected to be a standard wrapper type in the
        * same manner that reflection works; for example, int32_t would be
        * returned as a Integer32.
        */
        virtual Object::Holder extractFromEntry(Map::Entry::Holder ohEntry) const;


    // ----- QueryMapComparator interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                QueryMap::Entry::View vEntry2) const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Parse a comma-delimited sequence of method names and instantiate
        * a corresponding array ValueExtractor objects. Individual array
        * elements will be either ReflectionExtractor or ChainedExtractor
        * objects.
        *
        * @param vsNames  a comma-delimited sequence of method names
        *
        * @return an array of ValueExtractor objects
        */
        static ObjectArray::View createExtractors(String::View vsNames);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_MULTI_EXTRACTOR_HPP
