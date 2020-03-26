/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_TYPED_CLASS_HPP
#define COH_ABSTRACT_TYPED_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/ObjectArray.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* AbstractTypedClass provides an abstract templated implementation of Class.
*
* @author pf 2010.08.11
* @since Coherence 3.7
*/
template<class T> class AbstractTypedClass
    : public abstract_spec<AbstractTypedClass<T>,
        extends<Class> >
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Class type.
        */
        typedef T Type;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Initialize an AbstractTypedClass.
        */
        AbstractTypedClass<T>()
            : abstract_spec<AbstractTypedClass<T>, extends<Class> >(typeid(T))
            {
            }

    private:
        /**
        * Blocked copy constructor
        */
        AbstractTypedClass<T>(const AbstractTypedClass<T>&);


    // ----- AbstractTypedClass interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual const std::type_info& getTypeInfo() const
            {
            return typeid(T);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t getSize() const
            {
            return (size32_t) sizeof(T);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isInstance(Object::View v) const
            {
            return instanceof<typename T::View>(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isAssignableFrom(Class::View vClass) const
            {
            if (typeid(T) == typeid(Object))
                {
                return true;
                }

            while (vClass != NULL)
                {
                if (vClass == this)
                    {
                    return true;
                    }
                ObjectArray::View va = vClass->getInterfaces();
                for (size32_t i = 0, c = va->length; i < c; ++i)
                    {
                    if (va[i] == this)
                        {
                        return true;
                        }
                    }
                try
                    {
                    vClass = vClass->getSuperclass();
                    }
                catch (... /* ClassNotFound */)
                    {
                    return false;
                    }
                }

            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual Class::View getSuperclass() const
            {
            const std::type_info& ti = typeid(typename T::inherited);
            if (ti == typeid(Void<>))
                {
                return NULL;
                }
            return coh_loadClassByType(ti);
            }

#define COH_GET_INTERFACE(I) \
    if (typeid(typename T::interface_##I) != typeid(void)) \
        try \
            { \
            ha[i] = coh_loadClassByType(typeid(typename T::interface_##I)); \
            ++i; \
            } \
        catch (... /*ClassNotFound*/) {}

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::View getInterfaces() const
            {
            if (typeid(typename T::interface_1) == typeid(void))
                {
                // no interfaces
                return ObjectArray::create(0);
                }
            // else there can be up to 16 interfaces, see interface_spec

            ObjectArray::Handle ha = ObjectArray::create(16);
            size32_t i = 0;

            COH_GET_INTERFACE(1)
            COH_GET_INTERFACE(2)
            COH_GET_INTERFACE(3)
            COH_GET_INTERFACE(4)
            COH_GET_INTERFACE(5)
            COH_GET_INTERFACE(6)
            COH_GET_INTERFACE(7)
            COH_GET_INTERFACE(8)
            COH_GET_INTERFACE(9)
            COH_GET_INTERFACE(10)
            COH_GET_INTERFACE(11)
            COH_GET_INTERFACE(12)
            COH_GET_INTERFACE(13)
            COH_GET_INTERFACE(14)
            COH_GET_INTERFACE(15)
            COH_GET_INTERFACE(16)

            if (i == ha->length)
                {
                return ha;
                }
            return ObjectArray::copy(ha, 0, ObjectArray::create(i));
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_TYPED_CLASS_HPP
