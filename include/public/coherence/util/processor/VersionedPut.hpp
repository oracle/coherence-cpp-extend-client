/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VERSIONED_PUT_HPP
#define COH_VERSIONED_PUT_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Versionable.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;
using coherence::util::Versionable;


/**
* VersionedPut is an EntryProcessor that assumes that entry values implement
* Versionable interface and performs an InvocableMap::Entry#setValue
* operation if and only if the version of the specified value matches to the
* version of the current value. In case of the match, the VersionedPut will
* increment the version indicator before the value is updated.
*
* @author tb  2008.04.28
*/
class COH_EXPORT VersionedPut
    : public class_spec<VersionedPut,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<VersionedPut>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a VersionedPut.
        */
        VersionedPut();

        /**
        * Construct a VersionedPut that updates an entry with a new value if
        * and only if the version of the new value matches to the version of
        * the current entry's value. This processor optionally returns the
        * current value as a result of the invocation if it has not been
        * updated (the versions did not match).
        *
        * @param hValue        a value to update an entry with
        * @param fAllowInsert  specifies whether or not an insert should be
        *                      allowed (no currently existing value)
        * @param fReturn       specifies whether or not the processor should
        *                      return the current value in case it has not
        *                      been updated
        */
        VersionedPut(Versionable::Handle hValue, bool fAllowInsert = false,
                bool fReturn = false);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View processAll(Set::View vsetEntries) const;


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


    // ----- helper functions -----------------------------------------------

    protected:
        /**
        * Process the given entry.
        */
        virtual Object::Holder processEntry(
                InvocableMap::Entry::Handle hEntry,
                Versionable::Holder ohValueNew,
                bool fInsert,
                bool fReturn) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Specifies the new value to update an entry with.
        */
        mutable FinalHandle<Versionable> f_hValue;

        /**
        * Specifies whether or not an insert is allowed.
        */
        bool m_fInsert;

        /**
        * Specifies whether or not a return value is required.
        */
        bool m_fReturn;
     };

COH_CLOSE_NAMESPACE3

#endif // COH_VERSIONED_PUT_HPP
