/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_URI_HPP
#define COH_URI_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* Represents a Uniform Resource Identifier (URI) reference.
*
* At the highest level a URI reference (hereinafter simply "URI") in string
* form has the syntax
*
* [scheme:]scheme-specific-part[#fragment]
*
* where square brackets [...] delineate optional components and the
* characters : and # stand for themselves.
*
* @author jh 2007.12.20
*/
class COH_EXPORT URI
    : public class_spec<URI>
    {
    friend class factory<URI>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a URI by parsing the given RFC 2396-formatted string.
        *
        * @param vsUri  the String to be parsed as a URI
        *
        * @throws URISyntaxException if the given string violates RFC 2396
        */
        URI(String::View vsURI);

        /**
        * Create a new URI with the specified scheme, scheme-specific-part,
        * and fragment.
        *
        * @param vsScheme    the URI scheme
        * @param vsSSP       the URI scheme-specific-part
        * @param vsFragment  the URI fragment
        *
        * @throws URISyntaxException if the given scheme-specific-part is
        *         NULL or empty
        */
        URI(String::View vsScheme, String::View vsSSP, String::View vsFragment);


    // ----- URI interface --------------------------------------------------

    public:
        /**
        * Return the optional scheme associated with this URI.
        *
        * @return the URI scheme; may be NULL
        */
        virtual String::View getScheme() const;

        /**
        * Return the scheme-specific-part associated with this URI.
        *
        * @return the URI scheme
        */
        virtual String::View getSchemeSpecificPart() const;

        /**
        * Return the optional fragment associated with this URI.
        *
        * @return the URI fragment; may be NULL
        */
        virtual String::View getFragment() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The URI scheme.
        */
        FinalView<String> f_vsScheme;

        /**
        * The URI scheme-specific-part.
        */
        FinalView<String> f_vsSSP;

        /**
        * The URI fragment.
        */
        FinalView<String> f_vsFragment;

        /**
        * Cached hash code;
        */
        mutable size32_t m_nHash;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_URI_HPP
