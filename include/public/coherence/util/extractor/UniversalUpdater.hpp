/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNIVERSAL_UPDATER_HPP
#define COH_UNIVERSAL_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/extractor/AbstractUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
 * Universal ValueUpdater implementation.
 *
 * UniversalUpdater can only run within the Coherence cluster.
 *
 * Refer to the Coherence for Java documentation for more information.
 *
 * @author gg 2005.10.27, jf 2017.11.28, phf 2024.09.23
 *
 * @see CompositeUpdater
 *
 * @since 14.1.2.0.0
 */
class COH_EXPORT UniversalUpdater
    : public cloneable_spec<UniversalUpdater,
        extends<AbstractUpdater>,
        implements<PortableObject> >
    {
    friend class factory<UniversalUpdater>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct an empty UniversalUpdater
         * (necessary for the PortableObject interface).
         */
        UniversalUpdater();

        /**
         * Construct a UniversalUpdater for the provided name.
         *
         * @param vsName  a method or property name
         */
        UniversalUpdater(String::View vsName);

        /**
         * Copy constructor.
         */
        UniversalUpdater(const UniversalUpdater& that);

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
        virtual TypedHandle<const String> toString() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
         * A method name, or a property name.
         */
        FinalView<String> f_vsName;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_UNIVERSAL_UPDATER_HPP
