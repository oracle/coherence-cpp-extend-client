/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ANNOTATION_VISITOR_HPP
#define COH_ANNOTATION_VISITOR_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/Visitor.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)


/**
* An AnnotationVisitor is a Visitor implementation that injects
* information obtained by inspecting a provided class into a
* ClassMetadataBuilder. The modified builder will then realize a
* TypeMetadata instance with this injected information.
*
* This implementation is responsible purely for injecting explicitly defined
* information in the form of annotations. It depends upon, and hence is aware
* of, only the following annotations:
* <ul>
*      <li>Portable</li>
*      <li>PortableProperty</li>
* </ul>
*
* This class has one strategy of metadata discovery: accessor.
* Duplication is deemed by the same name derived by
* NameMangler implementations.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT AnnotationVisitor
    : public class_spec<AnnotationVisitor,
               extends<Object>,
               implements<Visitor> >
    {
    friend class factory<AnnotationVisitor>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct an AnnotationVisitor instance specifying whether to use
        * auto-indexing.
        *
        * @param fAutoIndex  whether to enable auto-indexing, default = false
        */
        AnnotationVisitor(bool fAutoIndex = false);

    // ----- Visitor interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void visit(Object::Holder ohBuilder, Class::View vClz) const;

    // ----- data members ---------------------------------------------------

    private:
        /**
        * Whether to use the auto-indexing feature to derive indexes.
        */
        bool m_fAutoIndex;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_ANNOTATION_VISITOR_HPP
