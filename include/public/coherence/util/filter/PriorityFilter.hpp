/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRIORITY_FILTER_HPP
#define COH_PRIORITY_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/net/AbstractPriorityTask.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::AbstractPriorityTask;


/**
* PriorityFilter is used to explicitly control the scheduling priority and
* timeouts for execution of filter-based methods.
* <p/>
* For example, let's assume that there is a cache that belongs to a partitioned
* cache service configured with a <i>request-timeout</i> and <i>task-timeout</i>
* of 5 seconds.
* Also assume that we are willing to wait longer for a particular rarely
* executed parallel query that does not employ any indexes. Then we could
* override the default timeout values by using the PriorityFilter as follows:
* <pre>
*   EqualsFilter::Handle   hFilterStandard = LikeFilter::create("getComments", "%fail%");
*   PriorityFilter::Handle hFilterPriority = PriorityFilter::create(hFilterStandard);
*   hFilterPriority->setExecutionTimeoutMillis(PriorityTask::timeout_none);
*   hFilterPriority->setRequestTimeoutMillis(PriorityTask::timeout_none);
*   Set::View vSetEntries = hCache->entrySet(hFilterPriority);
* </pre>
* <p/>
* This is an advanced feature which should be used judiciously.
*
* @author djl  2008.03.02
*/
class COH_EXPORT PriorityFilter
    : public class_spec<PriorityFilter,
        extends<AbstractPriorityTask>,
        implements<IndexAwareFilter> >
    {
    friend class factory<PriorityFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        PriorityFilter();

        /**
        * Construct a PriorityFilter.
        *
        * @param vFilter  the filter wrapped by this PriorityFilter
        */
        PriorityFilter(IndexAwareFilter::View vFilter);


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
        * Obtain the underlying filter.
        *
        * @return the filter wrapped by this PriorityFilter
        */
        virtual IndexAwareFilter::View getFilter() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The wrapped filter.
        */
        FinalView<IndexAwareFilter> f_vFilter;
    };

COH_CLOSE_NAMESPACE3

#endif //COH_PRIORITY_FILTER_HPP
