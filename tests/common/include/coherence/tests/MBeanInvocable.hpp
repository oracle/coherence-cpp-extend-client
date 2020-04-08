/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MBEAN_INVOCABLE_HPP
#define COH_MBEAN_INVOCABLE_HPP

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
* Example Invocable implementation that returns an MBean attribute.
*
* @author lh  2011.01.24
*/
class MBeanInvocable
    : public class_spec<MBeanInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<MBeanInvocable>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new MBeanInvocable instance.
        */
        MBeanInvocable();

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

#endif // COH_MBEAN_INVOCABLE_HPP
