/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_POF_PATH_HPP
#define COH_SIMPLE_POF_PATH_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/reflect/AbstractPofPath.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)


/**
* A static PofNavigator implementation which uses an array of integer indices
* to navigate the PofValue hierarchy.
*
* @author as/gm  2009.04.01
* @since Coherence 3.5
*/
class COH_EXPORT SimplePofPath
    : public class_spec<SimplePofPath,
        extends<AbstractPofPath> >
    {
    friend class factory<SimplePofPath>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        SimplePofPath();

        /**
        * Construct a SimplePofPath using a single index as a path.
        *
        * @param nIndex  an index
        */
        SimplePofPath(int32_t nIndex);

        /**
        * Construct a SimplePofPath using an array of indices as a path.
        *
        * @param vanIndices  an array of indices
        */
        SimplePofPath(Array<int32_t>::View vanIndices);


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual Array<int32_t>::View getPathElements() const;


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
        * Compare the SimplePofPath with another object to determine
        * equality. Two SimplePofPath objects are considered equal iff their
        * indices are equal.
        *
        * @param v  the object to compare against
        *
        * @return true iff this SimplePofPath and the passed object are
        *              equivalent.
        */
        virtual bool equals(Object::View v) const;

        /**
        * Determine a hash value for the SimplePofPath object according to
        * the general {@link Object#hashCode()} contract.
        *
        * @return an integer hash value for this SimplePofPath object
        */
        virtual size32_t hashCode() const;

        /**
        * Return a human-readable description for this SimplePofPath.
        *
        * @param out  the stream to write to
        *
        * @return a String description of the SimplePofPath
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------

    private:
        /**
        * Path elements.
        */
        FinalView<Array<int32_t> > f_vaiElements;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_SIMPLE_POF_PATH_HPP
