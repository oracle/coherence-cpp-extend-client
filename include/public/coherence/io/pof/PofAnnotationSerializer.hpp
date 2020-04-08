/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_ANNOTATION_SERIALIZER_HPP
#define COH_POF_ANNOTATION_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

/**
* A PofAnnotationSerializer provides annotation based (de)serialization.
* This serializer must be instantiated with the intended class which is
* eventually scanned for the presence of the following annotations.
* <ul>
*      <li>coherence::io::pof::annotation::Portable</li>
*      <li>coherence::io::pof::annotation::PortableProperty</li>
* </ul>
*
* This serializer supports classes iff they are annotated with the type level
* annotation; Portable. This annotation is a marker indicating the ability
* to (de)serialize using this serializer.
*
* All methods annotated with PortableProperty are explicitly
* deemed POF serializable with the option of specifying overrides to
* provide explicit behaviour such as:
* <ul>
*      <li>explicit POF indexes</li>
*      <li>Custom coherence::io::pof::reflect::Codec to
*          specify concrete implementations / customizations</li>
* </ul>
*
* The PortableProperty::getIndex() (POF index) can be omitted iff the
* auto-indexing feature is enabled. This is enabled by instantiating this
* class with the \c fAutoIndex constructor argument. This feature
* determines the index based on any explicit indexes specified and the name
* of the portable properties. Currently objects with multiple versions is
* not supported. The following illustrates the auto index algorithm:
* <table border=1>
*   <tr><td>Name</td><td>Explicit Index</td><td>Determined Index</td></tr>
*   <tr><td>c</td><td>1</td><td>1</td>
*   <tr><td>a</td><td></td><td>0</td>
*   <tr><td>b</td><td></td><td>2</td>
* </table>
*
* <b>NOTE:</b> This implementation does support objects that implement
* Evolvable.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*
* @see COH_REGISTER_TYPED_CLASS
* @see COH_REGISTER_POF_ANNOTATED_CLASS
* @see COH_REGISTER_POF_ANNOTATED_CLASS_AI
* @see Portable
*/
class COH_EXPORT PofAnnotationSerializer
    : public class_spec<PofAnnotationSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<PofAnnotationSerializer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Constructs a PofAnnotationSerializer.
        *
        * @param nTypeId     the POF type id
        * @param vClz        type this serializer is aware of
        * @param fAutoIndex  turns on the auto index feature, default = false
        */
        PofAnnotationSerializer(int32_t nTypeId, Class::View vClz, bool fAutoIndex = false);

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

    // ---- helpers ---------------------------------------------------------

    protected:
        /**
        * Initialize this class based on the provided information.
        *
        * @param nTypeId     the POF type id
        * @param vClz        type this serializer is aware of
        * @param fAutoIndex  turns on the auto index feature
        */
        virtual void initialize(int32_t nTypeId, Class::View vClz, bool fAutoIndex);

    // ---- data members ----------------------------------------------------

    private:
        /**
        * A structural definition of the type information.
        */
        FinalHandle<Object> f_ohTypeMetadata;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_ANNOTATION_SERIALIZER_HPP
