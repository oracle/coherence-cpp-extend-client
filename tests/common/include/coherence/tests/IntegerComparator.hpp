/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTEGER_COMPARATOR_HPP
#define COH_INTEGER_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/Comparator.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Comparator;

/**
* Comparator used for testing LimitFilter.
*
* @author ng  2011.1104.
*/
class IntegerComparator
    : public class_spec<IntegerComparator,
        extends<Object>,
        implements<Comparator, PortableObject> >
    {
    friend class factory<IntegerComparator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new IntegerComparator
        */
        IntegerComparator();


    // ---- Comparable interface --------------------------------------------

    public:
        /**
        * Compare two strings based on their length
        *
        * @throws NullPointerException if either argument is null
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const;


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

#endif // COH_INTEGER_COMPARATOR_HPP
