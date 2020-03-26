/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPARISON_FILTER_HPP
#define COH_COMPARISON_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Filter which compares the result of a method invocation with a value.
*
* @author djl  2008.03.02
*/
class COH_EXPORT ComparisonFilter
    : public abstract_spec<ComparisonFilter,
        extends<ExtractorFilter> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the ExternalizableLite interface).
        */
        ComparisonFilter();

        /**
        * Construct a ComparisonFilter.
        *
        * @param vExtractor the ComparisonFilter to use by this filter
        * @param vObject    the object to compare the result with
        */
        ComparisonFilter(ValueExtractor::View vValueExtractor,
                Object::View vObject);

    private:
        /**
        * Blocked copy constructor.
        */
        ComparisonFilter(const ComparisonFilter&);

		
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


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Helper method to calculate effectiveness for ComparisonFilters that
        * need no more than a single index match in order to retrieve all
        * necessary keys to perform the applyIndex() operation.
        * Such filters are: Contains, Equals, NotEquals.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by the
        *                     related ValueExtractor; read-only
        * @param vSetKeys     the set of keys that will be filtered; read-only
        *
        * @return an effectiveness estimate of how well this filter can use
        *         the specified indexes to filter the specified keys
        */
        virtual int32_t calculateMatchEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * Helper method to calculate effectiveness for ComparisonFilters that
        * need a range of values from an index in order to retrieve all
        * necessary keys to perform the applyIndex() operation.
        * Such filters are: Less, LessEquals, Greater, GreaterEquals.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by the
        *                     related ValueExtractor; read-only
        * @param vSetKeys     the set of keys that will be filtered; read-only
        *
        * @return an effectiveness estimate of how well this filter can use
        *         the specified indexes to filter the specified keys
        */
        virtual int32_t calculateRangeEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

    public:
        /**
        * Helper method to calculate effectiveness (or rather ineffectiveness)
        * of a simple iteration against a key set that has to be performed due
        * to an absence of corresponding index.
        *
        * @param cKeys  the number of keys to iterate through
        */
        static int32_t calculateIteratorEffectiveness(int32_t cKeys);


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
        * Get the object to compare the reflection result with.
        *
        * @return the object to compare the reflection result with
        */
        virtual Object::View getValue() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The value to compare to.
        */
        FinalView<Object> f_vValue;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPARISON_FILTER_HPP
