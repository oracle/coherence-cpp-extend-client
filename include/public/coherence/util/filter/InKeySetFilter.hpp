/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IN_KEY_SET_FILTER_HPP
#define COH_IN_KEY_SET_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Converter;


/**
* Filter that limits the underlying filter evaluation only to the specified
* set of keys.
*
* @author djl  2008.04.14
*/
class COH_EXPORT InKeySetFilter
    : public class_spec<InKeySetFilter,
        extends<Object>,
        implements<IndexAwareFilter, PortableObject> >
    {
    friend class factory<InKeySetFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        InKeySetFilter();

        /**
        * Construct an InSetFilter for testing "In" condition.
        *
        * @param vFilter   the underlying filter
        * @param vSetKeys  the set of keys to limit the filter evaluation to
        */
        InKeySetFilter(Filter::View vFilter, Set::View vSetKeys);


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the Filter.
        *
        * @return the Filter
        */
        virtual Filter::View getFilter() const;

        /**
        * Obtain the underying key set.
        *
        * @return the underlying key set
        *
        * @since 12.2.1
        */
        virtual Set::View getKeys() const;

    // ----- Helpers --------------------------------------------------------

    public:
        /**
        * Ensure that the underlying keys are converted using the specified
        * converter.
        *
        * This method must be called prior to index application or evaluation
        * when the keys being evaluated exist in an internal form.
        * 
        * Note: for convenience, this method will be automatically called by
        *       the partitioned cache service when this filter appears as the
        *       outermost filter in a query.
        *
        * @param vConverter  the converter that should be used for
        *                    key conversions
        */
        virtual void ensureConverted(Converter::View vConverter) const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The underlying Filter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The underlying set of keys.
        */
        mutable MemberView<Set> m_vSetKeys;

        /**
        * A flag that indicates that the key set has been converted to internal
        * form.
        */
        mutable bool m_fConverted;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_IN_KEY_SET_FILTER_HPP
