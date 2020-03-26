/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VERSIONED_OBJECT_HPP
#define COH_VERSIONED_OBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Versionable.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Versionable;


/**
* Simple Versionable implementation.
*
* @author tb  2008.06.09
*/
class VersionedObject
    : public class_spec<VersionedObject,
        extends<Object>,
        implements<Versionable, PortableObject> >
    {
    friend class factory<VersionedObject>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a VersionedObject.
        *
        * @param lVersion the version
        */
        VersionedObject(int64_t lVersion = 0);


    // ----- Versionable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Comparable::View getVersionIndicator() const;

        /**
        * {@inheritDoc}
        */
        virtual void incrementVersion();


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

    // ----- accessors ------------------------------------------------------

        virtual int64_t getVersion() const;

    // ----- data members ---------------------------------------------------

    protected:
        int64_t             m_lVersion;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VERSIONED_OBJECT_HPP
