/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_EXTRACTOR_HPP
#define COH_CONDITIONAL_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"
#include "coherence/util/extractor/IndexAwareExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::MapIndex;
using coherence::util::ValueExtractor;


/**
* An IndexAwareExtractor implementation that is only used to create a
* coherence::util::ConditionalIndex.
* <p/>
* Note: the underlying ValueExtractor is used for value extraction during
* index creation and is the extractor that is associated with the created
* coherence::util::ConditionalIndex in the given index map.  Using the
* ConditionalExtractor to extract values is not supported.
*
* @author tb/lh  2010.08.31
* @since Coherence 3.6
*/
class COH_EXPORT ConditionalExtractor
    : public class_spec<ConditionalExtractor,
        extends<AbstractExtractor>,
        implements<IndexAwareExtractor, PortableObject> >
    {
    friend class factory<ConditionalExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct the ConditionalExtractor.
        */
        ConditionalExtractor();

        /**
        * Construct the ConditionalExtractor.
        *
        * @param vFilter        the filter used by this extractor to create a
        *                       ConditionalIndex; must not be NULL
        * @param vExtractor     the extractor used by this extractor to create
        *                       a ConditionalIndex; Note that the created index
        *                       will be associated with this extractor in
        *                       the given index map; must not be NULL
        * @param fForwardIndex  specifies whether or not this extractor
        *                       will create a ConditionalIndex that
        *                       supports a forward map
        */
        ConditionalExtractor(Filter::View vFilter,
            ValueExtractor::View vExtractor,
            bool fForwardIndex);


    // ----- IndexAwareExtractor interface ----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual MapIndex::Handle createIndex(bool fOrdered, Comparator::View vComparator,
            Map::Handle hMapIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual MapIndex::Handle destroyIndex(Map::Handle hMapIndex) const;


    // ----- ValueExtractor interface ---------------------------------------

    public:
        /**
        * Using a ConditionalExtractor to extract values in not supported.
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


    // ----- Object methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The filter used by this extractor.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The underlying extractor.
        */
        FinalView<ValueExtractor> f_vExtractor;

        /**
        * Specifies whether or not this extractor will create a
        * coherence::util::ConditionalIndex that supports a forward index.
        */
        bool m_fForwardIndex;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONDITIONAL_EXTRACTOR_HPP
