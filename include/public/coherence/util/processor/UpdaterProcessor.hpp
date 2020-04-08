/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UPDATER_PROCESSOR_HPP
#define COH_UPDATER_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;
using coherence::util::ValueUpdater;


/**
* UpdaterProcessor is an EntryProcessor implementations that updates an
* attribute of an object cached in an InvocableMap.
*
* @author tb  2008.04.28
*/
class COH_EXPORT UpdaterProcessor
    : public class_spec<UpdaterProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<UpdaterProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a UpdaterProcessor.
        */
        UpdaterProcessor();

        /**
        * Construct an UpdaterProcessor based on the specified ValueUpdater.
        *
        * @param vUpdater  a ValueUpdater object; passing null will simpy
        *                  replace the entry's value with the specified one
        *                  instead of updating it
        * @param hValue    the value to update the target entry with
        */
        UpdaterProcessor(ValueUpdater::View vUpdater, Object::Holder hValue);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;


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
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying ValueUpdater.
        */
        FinalView<ValueUpdater> f_vUpdater;

        /**
        * A value to update the entry's value with.
        */
        FinalHolder<Object> f_hValue;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_UPDATER_PROCESSOR_HPP
