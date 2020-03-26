/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PROPERTY_MANIPULATOR_HPP
#define COH_PROPERTY_MANIPULATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueManipulator.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::ValueExtractor;
using coherence::util::ValueManipulator;
using coherence::util::ValueUpdater;


/**
* PropertyManipulator is a reflection based ValueManipulator implementation
* based on the JavaBean property name conventions.
*
* @author tb  2008.05.06
*/
class COH_EXPORT PropertyManipulator
    : public class_spec<PropertyManipulator,
        extends<Object>,
        implements<ValueManipulator, PortableObject> >
    {
    friend class factory<PropertyManipulator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a PropertyManipulator.
        */
        PropertyManipulator();

        /**
        * Construct a PropertyManipulator for the specified property name.
        * <p>
        * This constructor assumes that the corresponding property getter
        * will have a name of either ("get" + sName) or ("is + sName) and the
        * corresponding property setter's name will be ("set + sName).
        *
        * @param vsName a property name
        * @param fUseIs if true, the getter method will be prefixed with "is"
        *                rather than "get"
        */
        PropertyManipulator(String::View vsName, bool fUseIs = false);


    // ----- ValueManipulator interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ValueExtractor::View getExtractor() const;

        /**
        * {@inheritDoc}
        */
        virtual ValueUpdater::View getUpdater() const;


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * Parse the property name and initialize necessary extractor and
        * updator.
        */
        void init() const;


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

        /**
        * The property name, never null.
        */
        FinalView<String> f_vsName;

        /**
        * The getter prefix flag.
        */
        bool m_fUseIs;

        /**
        * A partial ValueExtractor used for composite properties.
        */
        mutable FinalView<ValueExtractor> f_vExtractorPart;

        /**
        * The underlying ValueExtractor.
        */
        mutable FinalView<ValueExtractor> f_vExtractor;

        /**
        * The underlying ValueUpdater.
        */
        mutable FinalView<ValueUpdater> f_vUpdater;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PROPERTY_MANIPULATOR_HPP
