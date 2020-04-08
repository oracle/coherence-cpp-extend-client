/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IN_FILTER_HPP
#define COH_IN_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which checks whether the result of a method invocation belongs to a
* predefined set of values.
*
* @author djl  2008.04.07
*/
class COH_EXPORT InFilter
    : public class_spec<InFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<InFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        InFilter();

        /**
        * Construct an InFilter for testing "In" condition.
        *
        * @param vExtractor  the ValueExtractor to use by this filter
        * @param vSetValues  the set of values to compare the result with
        */
        InFilter(ValueExtractor::View vExtractor, Set::View vSetValues);

    private:
        /**
        * Blocked copy constructor.
        */
        InFilter(const InFilter&);


    // ----- ExtractorFilter interface --------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateExtracted(Object::View vExtracted) const;


    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const;


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
   };

COH_CLOSE_NAMESPACE3

#endif // COH_IN_FILTER_HPP
