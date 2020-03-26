/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VALUE_CHANGE_EVENT_FILTER_HPP
#define COH_VALUE_CHANGE_EVENT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/filter/ExtractorFilter.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which evaluates the content of a MapEvent values based on the
* specified value extractor.  This filter evaluates to true only for update
* events that change the value of an extracted attribute.
*
* Example: a filter that evaluates to true if there is an update to an
* Employee object that changes a value of the LastName property.
* <pre>
*   ValueChangeEventFilter::create(ReflectionExtractor::create("LastName"));
* </pre>
*
* see MapEventFilter
*
* @author djl  2008.05.19
*/
class COH_EXPORT ValueChangeEventFilter
    : public class_spec<ValueChangeEventFilter,
        extends<ExtractorFilter> >
    {
    friend class factory<ValueChangeEventFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ValueChangeEventFilter();

        /**
        * Construct a ValueChangeEventFilter that evaluates MapEvent values
        * based on the specified extractor.
        *
        * @param vExtractor  ValueExtractor to extract MapEvent values
        */
        ValueChangeEventFilter(ValueExtractor::View vValueExtractor);

    private:
        /**
        * Blocked copy constructor.
        */
        ValueChangeEventFilter(const ValueChangeEventFilter&);


    // ----- ExtractorFilter interface --------------------------------------

    protected:
        /**
        * Evaluate the specified extracted value.
        *
        * @param vExtracted  an extracted value to evaluate
        */
        virtual bool evaluateExtracted(Object::View vExtracted) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


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
    };

COH_CLOSE_NAMESPACE3

#endif // COH_VALUE_CHANGE_EVENT_FILTER_HPP
