/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FLOAT64_AVERAGE_HPP
#define COH_FLOAT64_AVERAGE_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractFloat64Aggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* Calculates an average for values of any numberic type extracted from a set
* of entries in a Map. All the extracted Number objects will be treated as
* <tt>float64_t</tt> values. If the set of entries is empty, a
* <tt>NULL</tt> result is returned.
*
* @author djl  2008.05.09
*/
class COH_EXPORT Float64Average
    : public class_spec<Float64Average,
        extends<AbstractFloat64Aggregator> >
    {
    friend class factory<Float64Average>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        Float64Average();

        /**
        * Construct a Float64Average aggregator.
        *
        * @param vExtractor  the extractor that provides a value in the form
        *                    of any object that is a Number
        */
        Float64Average(ValueExtractor::View vExtractor);

        /**
        * Construct an Float64Average that will aggregate values extracted
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
        Float64Average(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        Float64Average(const Float64Average&);


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

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder finalizeResult(bool fFinal);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_FLOAT64_AVERAGE_HPP
