/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXTRACTOR_FILTER_HPP
#define COH_EXTRACTOR_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/EntryFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Base Filter implementation for doing extractor-based processing.
*
* @author djl  2008.03.02
*/
class COH_EXPORT ExtractorFilter
    : public abstract_spec<ExtractorFilter,
        extends<Object>,
        implements<EntryFilter, PortableObject> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ExtractorFilter();

        /**
        * Construct a ExtractorFilter for a given ValueExtractor.
        *
        * @param vExtractor the ValueExtractor to use by this filter
        */
        ExtractorFilter(ValueExtractor::View vExtractor);

    private:
        /**
        * Blocked copy constructor.
        */
        ExtractorFilter(const ExtractorFilter&);


    // ----- ExtractorFilter interface --------------------------------------

    protected:
        /**
        * Evaluate the specified extracted value.
        *
        * @param vExtracted  an extracted value to evaluate
        */
        virtual bool evaluateExtracted(Object::View vExtracted) const = 0;

        /**
        * Get the result of ValueExtractor invocation.
        *
        * @param v  the object on which to invoke the ValueExtractor;
        *           must not be NULL
        *
        * @return the result of the method invocation
        */
        virtual Object::View extract(Object::View v) const;


    // ----- EntryFilter interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


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
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the ValueExtractor used by this filter.
        *
        * @return the ValueExtractor::View used by this filter
        */
        virtual ValueExtractor::View getValueExtractor() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The evaluation cost as a factor to the single index access operation.
        *
        * @see IndexAwareFilter#calculateEffectiveness(Map, Set)
        */
        static const size32_t eval_cost = 1000;


   // ----- data members ---------------------------------------------------

    protected:
        /**
        * The ValueExtractor used by this filter.
        */
        FinalView<ValueExtractor> f_vExtractor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EXTRACTOR_FILTER_HPP
