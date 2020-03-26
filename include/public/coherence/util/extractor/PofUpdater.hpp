/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_UPDATER_HPP
#define COH_POF_UPDATER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/reflect/PofNavigator.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::reflect::PofNavigator;


/**
* POF-based ValueUpdater implementation.
*
* @author as/gm  2009.04.02
*
* @since Coherence 3.5
*/
class COH_EXPORT PofUpdater
    : public class_spec<PofUpdater,
        extends<Object>,
        implements<ValueUpdater, PortableObject> >
    {
    friend class factory<PofUpdater>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (for backward compatibility).
        */
        PofUpdater();

        /**
        * Constructs a PofUpdater based on a property index.
        * <p/>
        * This constructor is equivalent to:
        * <pre>
        *   PofUpdater::View vUpdater =
        *       PofUpdater::create(SimplePofPath::create(iProp));
        * </pre>
        *
        * @param iProp  property index
        */
        PofUpdater(int32_t iProp);

        /**
        * Constructs a PofUpdater based on a property path.
        *
        * @param vNavigator  POF navigator
        */
        PofUpdater(PofNavigator::View vNavigator);


   // ----- ValueUpdater interface ------------------------------------------

    public:
        /**
        * Update the passed target object using the specified value.
        * <p/>
        * It is expected that this updater will only be used against
        * POF-encoded entries implementing {@link BinaryEntry} interface.
        *
        * @param hTarget  the Object to update
        * @param ohValue  the new value to update the target's property with
        *
        * @throws UnsupportedOperationException  if the specified target
        *         object is not a POF-encoded {@link BinaryEntry} or the
        *         serializer is not a PofContext
        * @throws NullPointerException if the property cannot be resolved
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
        * Compare the PofUpdater with another object to determine
        * equality.
        *
        * @return true iff this PofUpdater and the passed object are
        *         equivalent PofUpdaters
        */
        virtual bool equals(Object::View v) const;

        /**
        * Determine a hash value for the PofUpdater object according to
        * the general {@link Object#hashCode()} contract.
        *
        * @return an integer hash value for this PofUpdater object
        */
        virtual size32_t hashCode() const;

        /**
        * Return a human-readable description for this PofUpdater.
        *
        * @return a String description of the PofUpdater
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * POF navigator.
        */
        FinalView<PofNavigator> f_vNavigator;
    };

COH_CLOSE_NAMESPACE3

#endif // #define COH_POF_UPDATER_HPP
