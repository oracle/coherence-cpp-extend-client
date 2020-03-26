/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTEGER64_SUM_HPP
#define COH_INTEGER64_SUM_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractInteger64Aggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* Sums up numeric values extracted from a set of entries in a Map. All the
* extracted Number objects will be treated as <tt>int64_t</tt> values.
*
* @author djl  2008.05.12
*/
class COH_EXPORT Integer64Sum
    : public class_spec<Integer64Sum,
        extends<AbstractInteger64Aggregator> >
    {
    friend class factory<Integer64Sum>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        Integer64Sum();

        /**
        * Construct a Integer64Sum aggregator.
        *
        * @param vExtractor  the extractor that provides a value in the form
        *                    of any object that is a Number
        */
        Integer64Sum(ValueExtractor::View vExtractor);

        /**
        * Construct an Integer64Sum that will aggregate values extracted
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
        Integer64Sum(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        Integer64Sum(const Integer64Sum&);


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

#endif // COH_INTEGER64_SUM_HPP
