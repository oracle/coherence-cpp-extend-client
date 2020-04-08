/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_EXTRACTOR_HPP
#define COH_POF_EXTRACTOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofConstants.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/reflect/PofNavigator.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::PofConstants;
using coherence::io::pof::PofContext;
using coherence::io::pof::PofHelper;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::reflect::PofNavigator;


/**
* POF-based ValueExtractor implementation.
* PofExtractor takes advantage of POF's indexed state to extract part of an
* object without needing to deserialize the entire object.
* <p/>
* POF uses a compact form in the serialized value when possible. For example,
* some numeric values are represented as special POF intrinsic types in which
* the type implies the value. As a result, POF requires the receiver of a
* value to have implicit knowledge of the type. PofExtractor uses the type
* supplied in the constructor as the source of the type information. If the
* type is void, PofExtractor will infer the type from the serialized state.
* See {@link PofConstants} for the list of the POF types.
* <p/>
* In C++ the type is expressed by type_info which is obtained from the typeid
* operator.
*
* Example where extracted value is float64_t:
* <pre><code>
*     ValueExtractor::Handle hExtractor =
*         PofExtractor::create(typeid(float64_t), 0);
* </code></pre>
*
* Example where extracted value should be inferred from the serialized state:
* <pre><code>
*     ValueExtractor::Handle hExtractor =
*         PofExtractor::create(typeid(void), 0);
* </code></pre>
*
* @author as/gm 2009.04.02
* @since Coherence 3.5
*/
class COH_EXPORT PofExtractor
    : public class_spec<PofExtractor,
        extends<AbstractExtractor>,
        implements<PortableObject> >
    {
    friend class factory<PofExtractor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        PofExtractor();

        /**
        * Constructs a PofExtractor based on a property index.
        * <p/>
        * This constructor is equivalent to:
        * <pre>
        *   PofExtractor::View vExtractor =
        *       PofExtractor::create(info, SimplePofPath::create(iProp), value);
        * </pre>
        *
        * @param info  the required type of the extracted value or void if
        *              the type is to be inferred from the serialized state
        * @param iProp  property index
        */
        PofExtractor(const std::type_info& info, int32_t iProp);

        /**
        * Constructs a PofExtractor based on a property path and the entry
        * extraction target.
        *
        * @param info        the required type of the extracted value or void
        *                    if the type is to be inferred from the serialized
        *                    state
        * @param vNavigator  POF navigator
        * @param nTarget     one of the {@link #value} or {@link #key} values
        */
        PofExtractor(const std::type_info& info, PofNavigator::View vNavigator,
                int32_t nTarget = value);


    // ----- AbstractExtractor methods --------------------------------------

    public:
        /**
        * Extract the value from the passed Entry object. The returned value
        * should follow the conventions outlined in the {@link #extract}
        * method.
        * <p/>
        * This method will always throw a
        * {@link UnsupportedOperationException} if called directly by the
        * C++ client application, as its execution is only meaningful within
        * the cluster.
        * <p/>
        * It is expected that this extractor will only be used against
        * POF-encoded binary entries within a remote partitioned cache.
        *
        * @param entry  an Entry object to extract a desired value from
        *
        * @throws UnsupportedOperationException  always, as it is expected
        *         that this extractor will only be executed within the
        *         cluster.
        *
        * @return the extracted value
        */
        virtual Object::Holder extractFromEntry(Map::Entry::Holder ohEntry) const;


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
        * Compare the PofExtractor with another object to determine equality.
        * Two PofExtractor objects are considered equal iff their paths are
        * equal and they have the same target (key or value).
        *
        * @return true iff this PofExtractor and the passed object are
        *              equivalent
        */
        virtual bool equals(Object::View v) const;

        /**
        * Determine a hash value for the PofExtractor object according to the
        * general {@link Object#hashCode()} contract.
        *
        * @return an integer hash value for this PofExtractor object
        */
        virtual size32_t hashCode() const;

        /**
        * Return a human-readable description for this PofExtractor.
        *
        * @return a String description of the PofExtractor
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the Class of the extracted value.
        *
        * @return the expected Class
        */
        Class::View getClassExtracted() const;

        /**
        * Obtain the PofPath for this extractor.
        *
        * @return the PofPath value
        */
        virtual PofNavigator::View getNavigator() const;


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Compute the expected POF type id based on the class.
        *
        * @param vCtx  POF context
        *
        * @return POF type id or t_unknown if the class is NULL.
        */
        virtual int32_t getPofTypeId(PofContext::View vCtx) const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * POF navigator.
        */
        FinalView<PofNavigator> f_vNavigator;

        /**
        * Class for what is being extracted; or NULL if this information is
        * specified in m_nType.
        */
        FinalView<Class> f_vClass;

        /**
        * POF type for expected value.
        */
        int32_t m_nType;
    };

COH_CLOSE_NAMESPACE3

#endif // #define COH_POF_EXTRACTOR_HPP
