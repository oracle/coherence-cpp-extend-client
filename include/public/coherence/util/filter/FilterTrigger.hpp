/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FILTER_TRIGGER_HPP
#define COH_FILTER_TRIGGER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapTrigger.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* A generic Filter-based MapTrigger implementation. If an evaluation of the
* coherence::util::MapTrigger::Entry object representing a pending
* change fails (returns false), then one of the following actions is taken:
* <ul>
*   <li> action_rollback - an IllegalArgumentException is thrown by the
*        trigger to reject the operation that would result in this change
*        (default);
*   <li> action_ignore - the change is ignored and the Entry's value is reset
*        to the original value returned by the
*        coherence::util::MapTrigger::Entry#getOriginalValue() method;
*   <li> action_remove - the entry is removed from the underlying backing map
*        using the coherence::util::InvocableMap::Entry#remove(bool) call.
* </ul>
*
* Note: This trigger never prevents entries from being removed.
*
* @author djl  2008.05.22
*/
class COH_EXPORT FilterTrigger
    : public class_spec<FilterTrigger,
        extends<Object>,
        implements<PortableObject, MapTrigger> >
    {
    friend class factory<FilterTrigger>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        FilterTrigger();

        /**
        * Construct a FilterTrigger based on the specified Filter object and
        * the action constant.
        *
        * @param vFilter  the underlying Filter
        * @param nAction  one of the ACTION_* constants, default is action_rollback
        */
        FilterTrigger(Filter::View vFilter, int32_t nAction = action_rollback);


    // ----- MapTrigger interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void process(MapTrigger::Entry::Handle vEntry);


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
        * Compare the FilterTrigger with another object to determine
        * equality. Two FilterTrigger objects are considered equal iff the
        * wrapped filters and action codes are equal.
        *
        * @return true iff this FilterTrigger and the passed object are
        *         equivalent FilterTrigger objects
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
        * Obtain the underlying Filter.
        *
        * @return the underlying Filter object
        */
        virtual Filter::View getFilter() const;

        /**
        * Obtain the action code for this FilterTrigger.
        *
        * @return one of the ACTION_* constants
        */
        virtual int32_t getAction() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Evaluation failure results in an IllegalArgumentException thrown
        * by the trigger.
        */
        static const int32_t action_rollback = 0;

        /**
        * Evaluation failure results in restoring the original Entry's value.
        */
        static const int32_t action_ignore = 1;

        /**
        * Evaluation failure results in a removal of the entry.
        */
        static const int32_t action_remove = 2;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying filter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The action code.
        */
        int32_t m_nAction;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_FILTER_TRIGGER_HPP
