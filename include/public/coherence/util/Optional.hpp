/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPTIONAL_HPP
#define COH_OPTIONAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
 * C++ port of java.util.Optional, a container object which may or may not
 * contain a non-NULL value.  If a value is present, isPresent() will return
 * true and get() will return the value.
 *
 * @author as,lh  2015.06.09
 * @since Coherence 12.2.1
 */
class COH_EXPORT Optional
    : public class_spec<Optional,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Optional>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Default constructor.
         */
        Optional();

    private:
        /**
         * Constructs an instance with the value present.
         *
         * @param voValue  the object value it wraps
         */
        Optional(Object::Holder oh);

        /**
         * Blocked copy constructor.
         */
        Optional(const Optional&);

    public:
        /**
         * Return an Optional instance with NULL object value.
         *
         * @return an Optional instance with NULL object value
         */
        static Optional::View empty();

        /**
         * Return an Optional instance with the given object value.
         *
         * @param voValue  the object value it wraps
         *
         * @return an Optional instance with the given object value
         */
        static Optional::View of(Object::Holder oh);

        /**
         * Return an Optional instance with the given value, if non-NULL.
         * Otherwise, return an empty Optional.
         *
         * @param oh  the object value it wraps
         *
         * @return an Optional instance with the given nullable value.
         */
        static Optional::View ofNullable(Object::Holder oh);

        /**
         * Return the object value.
         *
         * @return the object value
         */
        virtual Object::Holder get() const;

        /**
         * Return true if there is a value present, otherwise false.
         *
         * @return true if there is a value present, otherwise, false
         */
        virtual bool isPresent() const;

        /**
         * Return value if present, otherwise, the given object.
         *
         * @param oh the default object value to return
         *
         * @return the value if present, otherwise, return the given object
         */
        virtual Object::Holder orElse(Object::Holder oh) const;

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

    // ----- constants ------------------------------------------------------

    public:
        /**
         * Return an empty Optional instance.
         *
         * @return the empty Optional
         */
        static Optional::View emptyInstance();

        // ----- data members ---------------------------------------------------

    private:
        /**
         * The value this class wraps.  May be NULL.
         */
        FinalHolder<Object> f_oh;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OPTIONAL_HPP
