/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLONEABLE_SPEC_HPP
#define COH_CLONEABLE_SPEC_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/class_spec.hpp"
#include "coherence/lang/lang_spec.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

class Object;

/**
* Helper for defining a cloneable managed class.
*
* It addition to the features auto-generated by the class_spec<> helper template
* cloneable_spec<> auto-generates an implementation of "Object::Handle clone() const"
* which delegates to the defined classes const copy constructor.
*
* A normal cloneable class definition would be:
* @code
* class Foo
*   : public cloneable_spec<Foo,
*       extends<Bar>,
*       implements<SomeInterface, SomeOtherInterface> >
*   {
*   // add support for auto-generated static create methods
*   friend class factory<Foo>;
*
*   protected:
*       // Constructors are defined as protected, and access via
*       // auto-generated create methods, with matching signatures
*       Foo()
*           : super() ...
*           {
*           }
*
*       // Copy constructor
*       Foo(const Foo& that)
*           : super(that) ...
*           {
*           }
*
*   public:
*       // normal class definition....
*   };
* @endcode
*
* @see extends
* @see implements
*
* @author mf 2008.07.14
*/
template<class T, class E = extends<Object>, class I = implements<> >
class COH_EXPORT_SPEC cloneable_spec
    : public class_spec<T, E, I>
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Specification definition
        */
        typedef cloneable_spec this_spec;

        /**
        * Definition T's parent class
        */
        typedef cloneable_spec super;

        /**
        * Definition of the spec's parent class
        */
        typedef class_spec<T, E, I> super_spec;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Generate a set of proxy constructors matching the signatures of the
        * parent class's constructors.
        *
        * NOTE: Compilation errors referencing this line likely indicate that
        *       class being defined by this spec makes calls a "super"
        *       constructor supplying a set of parameters for which there is
        *       no exact match on the parent class.
        */
        COH_DEFINE_PROXY_CONSTRUCTORS(cloneable_spec)

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<Object> clone() const
            {
            return T::create(static_cast<const T&>(*this));
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLONEABLE_SPEC_HPP
