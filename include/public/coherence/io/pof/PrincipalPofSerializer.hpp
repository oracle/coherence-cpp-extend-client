/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRINCIPAL_POF_SERIALIZER_HPP
#define COH_PRINCIPAL_POF_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* PofSerializer implementation that can serialize and deserialize a
* Principal to/from a POF stream.
*
* The PrincipalPofSerializer can serialize any Principal implementation to a
* POF stream; however, the Principals returned during deserialization are
* always instances of the GenericPrincipal class.
*
* @author mf  2008.08.25
*/
class COH_EXPORT PrincipalPofSerializer
    : public class_spec<PrincipalPofSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<PrincipalPofSerializer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        PrincipalPofSerializer();


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

#endif // COH_PRINCIPAL_POF_SERIALIZER_HPP
