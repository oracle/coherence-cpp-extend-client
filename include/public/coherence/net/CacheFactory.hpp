/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_FACTORY_HPP
#define COH_CACHE_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"
#include "coherence/net/ConfigurableCacheFactory.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/run/xml/XmlElement.hpp"

#include <istream>
#include <sstream>

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlElement;
using coherence::io::IOException;


/**
* Factory for the <b>Oracle Coherence for C++</b> %cache product.
*
* One of the most common functions provided by the CacheFactory is ability to
* obtain an instance of a %cache. There are various %cache services and %cache
* topologies that are supported by Coherence.
*
* To get a %cache reference use getCache() method.
*
* When a %cache retrieved by any of the above methods is no longer used, it is
* preferable to call #releaseCache to release the associated resources. To
* destroy all instances of the %cache across the cluster, use #destroyCache.
*/
class COH_EXPORT CacheFactory
    : public abstract_spec<CacheFactory>
    {
    // ----- constructors ---------------------------------------------------

    private:
        /**
        * This constructor is blocked (private) as CacheFactory provides only
        * static interface and no instances may be created.
        */
        CacheFactory();


    // ----- caches ---------------------------------------------------------

    public:
        /**
        * Return an instance of a %cache configured by the current
        * ConfigurableCacheFactory. This helper method is a simple wrapper
        * around the ConfigurableCacheFactory#ensureCache method.
        *
        * @param vsName %cache name (unique for a given configurable %cache
        *              factory). If the NamedCache with the specified name
        *              already exists, a reference to the same object will be
        *              returned
        *
        * @return a handle to the NamedCache object
        */
        static NamedCache::Handle getCache(String::View vsName);

        /**
        * Releases and destroys the specified NamedCache.
        *
        * <b>Warning:</b> This method is used to completely destroy the
        * specified %cache across the cluster. All references in the entire
        * cluster to this %cache will be invalidated, the cached data will be
        * cleared, and all resources will be released.
        *
        * @param hCache the NamedCache object to be destroyed
        *
        * @see releaseCache
        */
        static void destroyCache(NamedCache::Handle hCache);

        /**
        * Release local resources associated with the specified instance of
        * the %cache.
        *
        * Releasing a NamedCache reference makes it no longer usable, but
        * does not affect the content of the %cache. In other words, all other
        * references to the %cache will still be valid, and the %cache data is
        * not affected by releasing the reference.
        *
        * The reference that is released using this method can no longer be
        * used; any attempt to use the reference will result in an exception.
        *
        * @param hCache the NamedCache object to be released
        *
        * @see destroyCache
        */
        static void releaseCache(NamedCache::Handle hCache);


    // ----- Service --------------------------------------------------------

    public:
        /**
        * Factory method returning an instance the named service.
        *
        * @param vsName  service name (unique across the cluster).  If the
        *                service with the specified name already exists, the
        *                reference to the same service will be returned.  If
        *                the name is not specified the default service name
        *                will be used
        *
        * @return an instance of the running service
        */
        static Service::Handle getService(String::View vsName);


    // ----- common ---------------------------------------------------------

    public:
        /**
        * Shutdown all clustered services.
        */
        static void shutdown();


    // ----- configuration --------------------------------------------------

    public:
        /**
        * Configure the CacheFactory and local member.
        *
        * @param vXmlCache      an XML element corresponding to cache-config.dtd
        * @param vXmlCoherence  an XML element corresponding to coherence.dtd
        *
        * @throws IllegalStateException if the factory has already been
        *         configured
        *
        * @see loadXml to read an XmlElement from an std::istream, String, or file
        */
        static void configure(XmlElement::View vXmlCache,
                XmlElement::View vXmlCoherence = NULL);

        /**
        * Read an XmlElement from the supplied stream.
        *
        * This method does not configure the CacheFactory, but it can be used
        * to obtain a configuration which can be supplied to the configure
        * method.
        *
        * @param in  the stream from which to read the XML element
        *
        * @return the XmlElement
        */
        COH_INLINE static XmlElement::Handle loadXml(std::istream& in)
            {
            if (in.fail())
                {
                COH_THROW_STREAM (IOException,
                        "Exception occurred during parsing: The stream "
                        << " is in a failed state.");
                }
            std::stringstream ss;
            char              caLine[256];

            while (!in.eof())
                {
                in.read(caLine, 255);
                caLine[in.gcount()] = 0x00;
                ss << caLine;
                }

            return loadXml(ss.str());
            }

        /**
        * Read an XmlElement from the supplied string.
        *
        * This method does not configure the CacheFactory, but it can be used
        * to obtain a configuration which can be supplied to the configure
        * method.
        *
        * @param vsXml  the string containing the XML
        *
        * @return the XmlElement
        */
        static XmlElement::Handle loadXml(String::View vsXml);

        /**
        * Read an XmlElement from the named file.
        *
        * This method does not configure the CacheFactory, but it can be used
        * to obtain a configuration which can be supplied to the configure
        * method.
        *
        * @param  vsFile name of the file to read from relative to the current
        *         working directory.
        *
        * @return the XmlElement
        */
        static XmlElement::Handle loadXmlFile(String::View vsFile);

        /**
        * Obtain the ConfigurableCacheFactory singleton.
        *
        * @return an instance of ConfigurableCacheFactory
        */
        static ConfigurableCacheFactory::Handle getConfigurableCacheFactory();

        /**
        * Specify a singleton of ConfigurableCacheFactory.
        *
        * @param hFactory  an instance of ConfigurableCacheFactory
        * @param vContext  an (optional) OperationalContext
        */
        static void setConfigurableCacheFactory(
                ConfigurableCacheFactory::Handle hFactory,
                OperationalContext::View vContext = NULL);

        /**
        * Invoke the Coherence C++ command line tool.
        */
        static void main(ObjectArray::View vasArg);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CACHE_FACTORY_HPP
