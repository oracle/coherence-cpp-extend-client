/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUBJECT_POF_SERIALIZER_HPP
#define COH_SUBJECT_POF_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* PofSerializer implementation that can serialize and deserialize a
* Subject to/from a POF stream.
*
* Only the Principals associated with the Subject are serialized.
*
* @author mf  2008.08.25
*
* @see PrincipalPofSerializer
*/
class COH_EXPORT SubjectPofSerializer
    : public class_spec<SubjectPofSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<SubjectPofSerializer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        SubjectPofSerializer();


    // ----- PofSerializer interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serialize(PofWriter::Handle hOut, Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder deserialize(PofReader::Handle hIn) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SUBJECT_POF_SERIALIZER_HPP
