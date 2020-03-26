/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_PERSON_MAPTRIGGER_HPP
#define COH_SIMPLE_PERSON_MAPTRIGGER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/MapTrigger.hpp"

#include "coherence/tests/SimplePerson.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::run::xml::XmlHelper;
using coherence::tests::SimplePerson;
using coherence::util::MapTrigger;


/**
* A MapTrigger for the SimplePerson class, implements a PortableObject.
*
* @author par  2011.07.14
* @since Coherence 12.1.3
*/
class SimplePersonMapTrigger
    : public class_spec<SimplePersonMapTrigger,
        extends<Object>,
        implements<MapTrigger, PortableObject> >
    {
    friend class factory<SimplePersonMapTrigger>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor
        */
        SimplePersonMapTrigger();

    // ----- MapTrigger interface -------------------------------------------

    public:
        /**
        * Method called before the result is committed.
        */
        virtual void process(MapTrigger::Entry::Handle hEntry);

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

#endif // COH_SIMPLE_PERSON_MAPTRIGGER_HPP
