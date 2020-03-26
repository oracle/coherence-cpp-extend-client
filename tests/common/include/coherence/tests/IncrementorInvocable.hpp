/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INCREMENTOR_INVOCABLE_HPP
#define COH_INCREMENTOR_INVOCABLE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/net/AbstractInvocable.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractInvocable;

/**
* Example Invocable implementation that returns the result of incrementing a
* given number.
*
* @author jh  2008.06.06
*/
class IncrementorInvocable
    : public class_spec<IncrementorInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<IncrementorInvocable>;

    // ----- constructors ---------------------------------------------------
    protected:
        /**
         * Default constructor - needed for PortableObject
         */
        IncrementorInvocable();

        /**
        * Create a new IncrementorInvocable instance.
        *
        * @param n  the number to increment
        */
        IncrementorInvocable(int32_t n);

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

    // ----- data members ---------------------------------------------------

    private:
        /**
        * The number to increment.
        */
        int32_t m_n;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INCREMENTOR_INVOCABLE_HPP
