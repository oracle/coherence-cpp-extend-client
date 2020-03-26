/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FLOAT64_MIN_HPP
#define COH_FLOAT64_MIN_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractFloat64Aggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* Calculates a minimum of numeric values extracted from a set of entries in
* a Map. All the extracted Number objects will be treated as
* <tt>int64_t</tt> values.
*
* @author djl  2008.05.12
*/
class COH_EXPORT Float64Min
    : public class_spec<Float64Min,
        extends<AbstractFloat64Aggregator> >
    {
    friend class factory<Float64Min>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        Float64Min();

        /**
        * Construct a Float64Min aggregator.
        *
        * @param vExtractor  the extractor that provides a value in the form
        *                    of any object that is a Number
        */
        Float64Min(ValueExtractor::View vExtractor);

        /**
        * Construct an Float64Min that will aggregate values extracted
        * from a set of InvocableMap::Entry objects.
        *
        * @param vsMethod  the name of the method that could be invoked via
        *                  reflection and that returns values to aggregate;
        *                  this parameter can also be a dot-delimited 
        *                  sequence of method names which would result in
        *                  an aggregator based on the ChainedExtractor that
        *                  is based on an array of corresponding 
        *                  ReflectionExtractor objects
        *
        * @since Coherence 12.1.2
        */
        Float64Min(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        Float64Min(const Float64Min&);


    // ----- AbstractAggregator Interface  ----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void init(bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual void process(Object::View v, bool fFinal);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_FLOAT64_MIN_HPP
