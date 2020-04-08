/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VERSIONED_PUT_ALL_HPP
#define COH_VERSIONED_PUT_ALL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;
using coherence::util::Map;


/**
* VersionedPutAll is an EntryProcessor that assumes that entry values
* implement Versionable interface and performs an
* InvocableMap::Entry#setValue operation only for entries whose versions
* match to versions of the corresponding current values. In case of the
* match, the VersionedPutAll will increment the version indicator before
* each value is updated.
*
* @author tb  2008.04.28
*/
class COH_EXPORT VersionedPutAll
    : public class_spec<VersionedPutAll,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<VersionedPutAll>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a VersionedPutAll.
        */
        VersionedPutAll();

        /**
        * Construct a VersionedPutAll processor that updates an entry with a
        * new value if and only if the version of the new value matches to
        * the version of the current entry's value (which must exist). This
        * processor optionally returns a map of entries that have not been
        * updated (the versions did not match).
        *
        * @param vMap          a map of values to update entries with
        * @param fAllowInsert  specifies whether or not an insert should be
        *                      allowed (no currently existing value)
        * @param fReturn       specifies whether or not the processor should
        *                      return the entries that have not been updated
        */
        VersionedPutAll(Map::View vMap, bool fAllowInsert = false,
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
            Map::View vMap,
            bool fInsert,
            bool fReturn) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Specifies the map of new values.
        */
        FinalView<Map> f_vMap;

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

#endif // COH_VERSIONED_PUT_ALL_HPP
