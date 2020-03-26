/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPOSITE_UPDATER_HPP
#define COH_COMPOSITE_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* A ValueManipulator implementation based on an extractor-updater pair.
*
* @author djl 2008.04.09
*
* @see CompositeUpdater
*/
class COH_EXPORT CompositeUpdater
    : public class_spec<CompositeUpdater,
        extends<Object>,
        implements<ValueUpdater, PortableObject> >
    {
    friend class factory<CompositeUpdater>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an empty CompositeUpdater
        * (necessary for the PortableObject interface).
        */
        CompositeUpdater();

        /**
        * Construct a CompositeUpdater for a specified method name sequence.
        *
        * For example: "getAddress. setZip" method name will indicate that the
        * "getAddress()" method should be used to extract an Address object,
        * which will then be used by the "setZip(String)" call.
        *
        * @param vsName  a dot-delimited sequence of N method names which results
        *                in a CompositeUpdater that is based on an chain of
        *                (N-1) ReflectionExtractor objects and a single
        *                ReflectionUpdater.
        */
        CompositeUpdater(String::View vsName);

        /**
        * Construct a CompositeUpdater based on the specified extractor and
        * updater.
        *
        * <b>Note:</b> the extractor and updater here are not symmetrical in
        * nature: the extractor is used to "drill-down" to the target object,
        * while the updater will operate on that extracted object.
        *
        * @param vExtractor  the ValueExtractor
        * @param vUpdater    the ValueUpdater
        */
        CompositeUpdater(ValueExtractor::View vExtractor,
                ValueUpdater::View vUpdater);


   // ----- ValueUpdater interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void update(Object::Handle hTarget,
                Object::Holder ohValue) const;


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


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Retrieve the ValueExtractor part.
        *
        * @return the ValueExtractor
        */
        virtual ValueExtractor::View getExtractor() const;

        /**
        * Retrieve the ValueUpdator part.
        *
        * @return the ValueUpdater
        */
        virtual ValueUpdater::View getUpdater() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The ValueExtractor part.
        */
        FinalView<ValueExtractor> f_vExtractor;

        /**
        * The ValueUpdaterr part.
        */
        FinalView<ValueUpdater> f_vUpdater;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPOSITE_UPDATER_HPP
