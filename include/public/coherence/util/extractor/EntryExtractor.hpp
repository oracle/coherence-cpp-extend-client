/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENTRY_EXTRACTOR_HPP
#define COH_ENTRY_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::QueryMap;


/**
* The EntryExtractor is a base abstract class for special purpose custom
* ValueExtractor implementations. It allows them to extract a desired value
* using all available information on the corresponding QueryMap.Entry object
* and is intended to be used in advanced custom scenarios, when application
* code needs to look at both key and value at the same time or can make some
* very specific assumptions regarding to the implementation details of the
* underlying Entry object.
*
* @author nsa 2008.08.07
*/
class COH_EXPORT EntryExtractor
    : public abstract_spec<EntryExtractor,
        extends<AbstractExtractor>,
        implements<PortableObject> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (for backward compatibility).
        */
        EntryExtractor();

        /**
        * Construct an EntryExtractor based on the entry extraction target.
        *
        * @param nTarget  one of the {@link #value} or {@link #key} values
        *
        * @since Coherence 3.5
        */
        EntryExtractor(int32_t nTarget);


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

COH_CLOSE_NAMESPACE3

#endif // #define COH_ENTRY_EXTRACTOR_HPP
