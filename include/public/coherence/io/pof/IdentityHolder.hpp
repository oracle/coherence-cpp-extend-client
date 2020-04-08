/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IDENTITY_HOLDER_HPP
#define COH_IDENTITY_HOLDER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofBufferReader.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* A thread local storage to hold the identity of an object and its corresponding
* POF stream reader.
*
* @author lh  2011.07.19
*
* @since Coherence 3.7.1
*/
class COH_EXPORT IdentityHolder
      : public class_spec<IdentityHolder,
        extends <Object> >
    {
    friend class factory<IdentityHolder>;

    // ----- member methods -------------------------------------------------

    public:
        /**
        * Store an identity for an object a POF stream reader just read.
        *
        * @param hReader  the POF stream reader
        * @param nId      the identity of an object
        */
        static void set(PofBufferReader::Handle hReader, int32_t nId);

        /**
        * Register an object.
        *
        * @param hReader  the POF stream reader
        * @param nId      the identity of an object
        * @param oh       the object
        */
        static void reset(PofBufferReader::Handle hReader, int32_t nId, Object::Holder oh);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_IDENTITY_HOLDER_HPP

