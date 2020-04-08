/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NULL_FILTER_HPP
#define COH_NULL_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* Filter which discards null references.
*
* @author gm  2008.07.28
*/
class COH_EXPORT NullFilter
    : public cloneable_spec<NullFilter,
        extends<Object>,
        implements<Filter, PortableObject> >
    {
    friend class factory<NullFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (for PortableObject).
        */
        NullFilter();

        /**
        * Copy constructor.
        */
        NullFilter(const NullFilter& that);


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
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NULL_FILTER_HPP
