/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_EVENT_FILTER_HPP
#define COH_MAP_EVENT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Filter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Filter which evaluates the content of a MapEvent object according to the
* specified criteria.  This filter is intended to be used by various
* ObservableMap listeners that are interested in particular subsets
* of MapEvent notifications emitted by the map.
*
* Usage examples:
* <ul>
* <li>a filter that evaluates to true if an Employee object is inserted into
*     a cache with a value of Married property set to true.
* <pre>
*   MapEventFilter::create(MapEventFilter.E_INSERT,
*       EqualsFilter::create(ReflectionExtractor::create("isMarried"),
*               Boolean::create(true));
* </pre>
* <li>a filter that evaluates to true if any object is removed from a cache.
* <pre>
*   MapEventFilter::create(MapEventFilter::E_DELETED);
* </pre>
* <li>a filter that evaluates to true if there is an update to an Employee
*     object where either an old or new value of LastName property equals to
*     "Smith"
* <pre>
*   MapEventFilter::create(MapEventFilter::E_UPDATE,
*       EqualsFilter::create(ReflectionExtractor::create("LastName"),
*               String::create("Smith")));
* </pre>
* <li>a filter that is used to keep a cached keySet result based on some map
*     filter up-to-date.
* <pre>
*   Set::View    setKeys   = HashSet::create();
*   Filter::View filterEvt = MapEventFilter::create(filterMap);
*   MapListener::Handle listener = MapListener::create();
*   map.addMapListener(listener, filterEvt, true);
*   setKeys.addAll(map.keySet(filterMap));
* </pre>
* </ul>
*
* see ValueChangeEventFilter
*
* @author djl  2008.05.19
*/
class COH_EXPORT MapEventFilter
    : public class_spec<MapEventFilter,
        extends<Describable>,
        implements<PortableObject, Filter> >
    {
    friend class factory<MapEventFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        MapEventFilter();

        /**
        * Construct a MapEventFilter that evaluates MapEvent objects that
        * would affect the results of a keySet filter issued by a previous
        * call to
        * coherence::util::QueryMap::keySet(coherence::util::Filter::View).
        *  It is possible to easily implement <i>continuous query</i>
        * functionality.
        *
        * Using this constructor is equivalent to:
        * <tt>
        * MapEventFilter::create(e_keyset, vFilter);
        * </tt>
        *
        * @param vFilter  the filter passed previously to a keySet() query
        * method
        */
        MapEventFilter(Filter::View vFilter);

        /**
        * Construct a MapEventFilter that evaluates MapEvent objects
        * based on the specified combination of event types.
        *
        * @param nMask    combination of any of the e_* values
        * @param vFilter  (optional) the filter used for evaluating event
        *                 values
        */
        MapEventFilter(int32_t nMask, Filter::View vFilter = NULL);

    private:
        /**
        * Blocked copy constructor.
        */
        MapEventFilter(const MapEventFilter&);


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
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the event mask. The mask value is concatenation of any of
        * the E_* values.
        *
        * @return the event mask
        */
        virtual int32_t getEventMask() const;


        /**
        * Obtain the Filter object used to evaluate the event value(s).
        *
        * @return the filter used to evaluate the event value(s)
        */
        virtual Filter::View getFilter() const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * This value indicates that MapEvent#entry_inserted
        * entry_inserted} events should be evaluated. The event will be fired
        *  if there is no filter specified or the filter evaluates to true
        * for a new value.
        */
        static const int32_t e_inserted       = 0x0001;

        /**
        * This value indicates that MapEvent#entry_updated entry_updated
        * events should be evaluated. The event will be fired if there is no
        * filter specified or the filter evaluates to true when applied to
        * either old or new value.
        */
        static const int32_t e_updated        = 0x0002;

        /**
        * This value indicates that MapEvent#entry_deleted entry_deleted
        * events should be evaluated. The event will be fired if there is no
        * filter specified or the filter evaluates to true for an old value.
        */
        static const int32_t e_deleted        = 0x0004;

        /**
        * This value indicates that MapEvent#entry_updated entry_updated
        * events should be evaluated, but only if filter evaluation is false
        * for the old value and true for the new value. This corresponds to
        * an item that was not in a keySet filter result changing such that
        * it would now be in that keySet filter result.
        */
        static const int32_t e_updated_entered = 0x0008;

        /**
        * This value indicates that MapEvent#entry_updated entry_updated
        * events should be evaluated, but only if filter evaluation is true
        * for the old value and false for the new value. This corresponds to
        * an item that was in a keySet filter result changing such that it
        * would no longer be in that keySet filter result.
        */
        static const int32_t e_updated_left   = 0x0010;

        /**
        * This value indicates that MapEvent#entry_updated entry_updated
        * events should be evaluated, but only if filter evaluation is true
        * for both the old and the new value. This corresponds to an item
        * that was in a keySet filter result changing but not leaving the
        * keySet filter result.
        */
        static const int32_t e_updated_within = 0x0020;

        /**
        * This value indicates that all events should be evaluated.
        */
        static const int32_t e_all            = e_inserted | e_updated |
                                                e_deleted;

        /**
        * This value indicates that all events that would affect the result
        * of a coherence::util::QueryMap::keySet(coherence::util::Filter)
        * query should be evaluated.
        */
        static const int32_t e_keyset         = e_inserted | e_deleted |
                                                e_updated_entered  |
                                                e_updated_left;
        /**
        * Event id to event mask translation.
        *
        * @param iEvent  the event index
        *
        * @return the translated event mask
        */
        static int32_t getMask(size32_t iEvent);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The event mask.
        */
        int32_t m_nMask;

        /**
        * The event value(s) filter.
        */
        FinalView<Filter> f_vFilter;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_MAP_EVENT_FILTER_HPP
