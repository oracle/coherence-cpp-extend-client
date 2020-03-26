/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXTRACTOR_EVENT_TRANSFORMER_HPP
#define COH_EXTRACTOR_EVENT_TRANSFORMER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapEventTransformer.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,transformer)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* ExtractorEventTransformer is a special purpose MapEventTransformer
* implementation that transforms emitted events, extracting one or more
* properties from either the OldValue or the NewValue. This transformation
* will generally result in the change of the values' data type.
* <p/>
* Example: the following code will register a listener to receive events only
* if the value of the AccountBalance property changes. The transformed
* event's NewValue will be a List containing the LastTransactionTime and
* AccountBalance properties. The OldValue will always be null.
* <pre>
*    Filter::View vFilter =
*       ValueChangeEventFilter::create("getAccountBalance");
*    ValueExtractor::View vExtractor =
*       MultiExtractor::create(MultiExtractor::createExtractors(
*           "getLastTransactionTime,getAccountBalance"));
*    MapEventTransformer::Handle hTransformer =
*       ExtractorEventTransformer::create(NULL, vExtractor);
*
*    cache->addMapListener(listener,
*       MapEventTransformerFilter::create(vFilter, hTransformer), false);
* </pre>
*
* @author djl  2008.06.17
*/
class COH_EXPORT ExtractorEventTransformer
    : public class_spec<ExtractorEventTransformer,
        extends<Object>,
        implements<PortableObject, MapEventTransformer> >
    {
    friend class factory<ExtractorEventTransformer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ExtractorEventTransformer();

        /**
        * Construct a ExtractorEventTransformer that transforms MapEvent
        * values based on the specified extractor.
        *
        * Note: The specified extractor will be applied to both old and new
        * values.
        *
        * @param vExtractor  ValueExtractor to extract MapEvent values
        */
        ExtractorEventTransformer(ValueExtractor::View vExtractor);

        /**
        * Construct a ExtractorEventTransformer that transforms MapEvent
        * values based on the specified extractors. Passing null indicates
        * that the corresponding values should be skipped completely.
        *
        * @param vExtractorOld  extractor to extract the OldValue property(s)
        * @param vExtractorNew  extractor to extract the NewValue property(s)
        */
        ExtractorEventTransformer(ValueExtractor::View vExtractorOld,
                ValueExtractor::View vExtractorNew);


    // ----- MapEventTransformer interface ----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual MapEvent::Handle transform(MapEvent::Handle hEvent) const;


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

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Return a ValueExtractor used to transfrom the event's OldValue.
        *
        * @return an extractor from the OldValue
        */
        virtual ValueExtractor::View getOldValueExtractor() const;

        /**
        * Return a ValueExtractor used to transfrom the event's NewValue.
        *
        * @return an extractor from the NewValue
        */
        virtual ValueExtractor::View getNewValueExtractor() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The OldValueExtractor.
        */
        FinalView<ValueExtractor> f_vExtractorOld;

        /**
        * The NewValueExtractor.
        */
        FinalView<ValueExtractor> f_vExtractorNew;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EXTRACTOR_EVENT_TRANSFORMER_HPP
