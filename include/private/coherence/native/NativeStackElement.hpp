/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_STACK_ELEMENT_HPP
#define COH_NATIVE_STACK_ELEMENT_HPP

#include "coherence/lang.ns"

#include "coherence/lang/StackTraceElement.hpp"

#include "private/coherence/util/StringHelper.hpp"



COH_OPEN_NAMESPACE2(coherence,native)

using coherence::util::StringHelper;


/**
* NativeStackElement provides native stack information.
*
* @author mf  2008.04.01
*/
class COH_EXPORT NativeStackElement
    : public class_spec<NativeStackElement,
        extends<Object>,
        implements<StackTraceElement> >
    {
    friend class factory<NativeStackElement>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NativeStackElement.
        *
        * @param vsFile    the file name associated with the frame
        * @param vsSymbol  the symbol name associated with the frame
        *
        * @return a new NativeStackElement
        */
        NativeStackElement(String::View vsFile,
                String::View vsSymbol)
                : f_vsFile(self(), StringHelper::stripSource(vsFile)),
                  f_vsSymbol(self(), vsSymbol)
            {
            }


    // ----- NativeStackElement interface -----------------------------------

    public:
        /**
        * Return the name of the file associated with the frame.
        *
        * @return the name of the file associated with the frame.
        */
        virtual String::View getFile() const
            {
            return f_vsFile;
            }

        /**
        * Return the demangled symbol name associated with the frame.
        *
        * @return the demangled symbol name associated with the frame
        */
        virtual String::View getSymbol() const
            {
            return f_vsSymbol;
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            String::View vsSymbol = getSymbol();

            if (NULL == vsSymbol || vsSymbol->length() == 0)
                {
                return String::create("<stack frame symbol unavailable>");
                }
            else
                {
                return COH_TO_STRING("at " << vsSymbol);
                }
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The file name.
        */
        FinalView<String> f_vsFile;

        /**
        * The symbol name.
        */
        FinalView<String> f_vsSymbol;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_STACK_ELEMENT_HPP
