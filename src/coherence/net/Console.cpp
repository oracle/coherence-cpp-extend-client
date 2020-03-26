/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/Console.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MultiplexingMapListener;
using coherence::util::Set;
using coherence::util::StringHelper;

COH_OPEN_NAMESPACE2(coherence,net)


// ----- file local helpers -------------------------------------------------

/**
* Helper macro for writting content to the logger at level 0, which will
* avoid logger headers
*
* TODO: enable if logger supports prompting at some point
*/
#define OUT(CONTENT) std::cout << CONTENT << std::endl

COH_OPEN_NAMESPACE_ANON(Console)

/**
* VerboseMapListener prints MapEvents.
*/
class VerboseMapListener
    : public class_spec<VerboseMapListener,
        extends<MultiplexingMapListener> >
    {
    friend class factory<VerboseMapListener>;

    // ----- MultiplexingMapListener interface ----------------------

    public:
        virtual void onMapEvent(MapEvent::View vEvent)
            {
            OUT(vEvent);
            }
    };


Object::View getNext(std::istream& in)
    {
    std::string s;
    in >> s;
    const char* ach = s.c_str();

    // handle well known constants
    if (strcmp(ach, "[true]") == 0)
        {
        return Boolean::create(true);
        }
    else if (strcmp(ach, "[false]") == 0)
        {
        return Boolean::create(false);
        }
    else if (strcmp(ach, "[null]") == 0)
        {
        return NULL;
        }

    // scan the string to identify the type
    bool fNumeric = true;
    bool fFloat   = false;
    for (size32_t i = 0; fNumeric; ++i)
        {
        char ch = ach[i];
        if (ch == 0)
            {
            fNumeric &= i > 0;
            break;
            }
        else if (ch == '.')
            {
            fNumeric &= !fFloat;
            fFloat    = true;
            }
        else if (isalpha(ch))
            {
            fNumeric = false;
            }
        }

    if (fNumeric)
        {
        if (fFloat)
            {
            return Float32::create(Float32::parse(ach));
            }
        else
            {
            return Integer32::create(Integer32::parse(ach));
            }
        }
    else if (ach[0] == 0)
        {
        return NULL;
        }

    // treat it as a String
    return String::create(ach);
    }

COH_CLOSE_NAMESPACE_ANON

void Console::main(ObjectArray::View vasArg)
    {
    NamedCache::Handle           hCache;
    HeapAnalyzer::Snapshot::View vSnap;
    size64_t                     lcbCacheSnap = 0;
    std::istream*                pStream = &std::cin;

    if (vasArg->length > 0)
        {
        // load command line specified cache
        try
            {
            String::View vsArg = cast<String::View>(vasArg[0]);
            if (vsArg->length() > 1 && vsArg->startsWith("@"))
                {
                // first command
                std::stringstream* pSStream = new std::stringstream();
                pStream = pSStream;
                *pSStream << vsArg->substring(1); // trim "@"
                for (size32_t i = 1, c = vasArg->length; i < c; ++i)
                    {
                    *pSStream << ' ' << vasArg[i];
                    }
                *pSStream << std::ends;
                }
            else
                {
                // load cache
                hCache = CacheFactory::getCache(vsArg);
                }
            }
        catch (Exception::View e)
            {
            OUT(e);
            }
        }

    while (true)
        {
        try
            {
            // prompt for input
            if (NULL == hCache)
                {
                std::cout << "\nMap (?): " << std::flush;
                }
            else
                {
                std::cout << "\nMap (" << hCache->getCacheName() << "): "  << std::flush;
                }

            char achInput[256];
            pStream->getline(achInput, 256);

            if (pStream->fail())
                {
                if (pStream == &std::cin)
                    {
                    pStream->clear();
                    }
                else
                    {
                    delete pStream;
                    pStream = &std::cin;
                    }
                continue;
                }

            std::stringstream ssInput(achInput);

            // process input
            String::View vsCmd = cast<String::View>(getNext(ssInput));

            if (vsCmd == NULL)
                {
                continue;
                }
            else if (vsCmd->equals("batch"))
                {
                if (pStream != &std::cin && !pStream->eof())
                    {
                    std::cerr << "nested scripts are unsupported";
                    }
                else
                    {
                    String::View vsFile = cast<String::View>(getNext(ssInput));
                    if (pStream != &std::cin)
                        {
                        delete pStream;
                        }
                    pStream = new std::ifstream(vsFile->getCString());
                    if (pStream->fail())
                        {
                        std::cerr << "error opening: " << vsFile << std::endl;
                        delete pStream;
                        pStream = &std::cin;
                        }
                    }
                }
            else if (vsCmd->equals("bye"))
                {
                // quit
                if (pStream != &std::cin)
                    {
                    delete pStream;
                    pStream = &std::cin;
                    }
                CacheFactory::shutdown();
                break;
                }
            else if (vsCmd->equals("cache"))
                {
                // lookup a cache from the CacheFactory
                String::View vsCacheName = cast<String::View>(getNext(ssInput));
                if (vsCacheName == NULL)
                    {
                    OUT(hCache);
                    }
                else
                    {
                    hCache = CacheFactory::getCache(vsCacheName);
                    }
                }
            else if (vsCmd->equals("classes"))
                {
                // output the SystemClassLoader
                OUT(SystemClassLoader::getInstance());
                }
            else if (vsCmd->equals("clear"))
                {
                // clear the current cache
                hCache->clear();
                }
            else if (vsCmd->equals("destroy"))
                {
                // destroy the current cache
                CacheFactory::destroyCache(hCache);
                }
            else if (vsCmd->equals("get"))
                {
                // perform a get operation on the current cache
                Object::View vKey   = getNext(ssInput);
                Object::View vValue = hCache->get(vKey);

                // print the current value
                OUT(vValue);
                }
            else if (vsCmd->equals("list"))
                {
                // obtain the entire cache contents
                Set::View vSetEntries = hCache->entrySet();

                // print key value pairs
                for (Iterator::Handle hIter = vSetEntries->iterator();
                        hIter->hasNext(); )
                    {
                    Map::Entry::View vEntry =
                        cast<Map::Entry::View>(hIter->next());

                    OUT(vEntry->getKey() << " = " << vEntry->getValue());
                    }
                }
            else if (vsCmd->equals("listen"))
                {
                String::View        vsAction  = cast<String::View>(getNext(ssInput));
                MapListener::Handle hListener = VerboseMapListener::create();

                if (vsAction != NULL && vsAction->equals("stop"))
                    {
                    Object::View vKey = getNext(ssInput);

                    if (vKey == NULL)
                        {
                        hCache->removeFilterListener(hListener, NULL);
                        }
                    else
                        {
                        hCache->removeKeyListener(hListener, vKey);
                        }
                    }
                else // start
                    {
                    Object::View vKey =
                        vsAction == NULL || vsAction->equals("start")
                            ? getNext(ssInput)
                            : (Object::View) vsAction;

                    if (vKey == NULL)
                        {
                        hCache->addFilterListener(hListener, NULL, false);
                        }
                    else
                        {
                        hCache->addKeyListener(hListener, vKey, false);
                        }
                    }
                }
            else if (vsCmd->equals("load"))
                {
                // load a dynamic library into this process
                System::loadLibrary(cast<String::View>(getNext(ssInput)));
                }
            else if (vsCmd->equals("memory"))
                {
                System::gc(/*fFull*/ true);

                // print information about allocated objects
                HeapAnalyzer::View vAnalyzer = System::getHeapAnalyzer();

                vsCmd = cast<String::View>(getNext(ssInput));
                bool fDelta = false;
                if (NULL == vAnalyzer)
                    {
                    OUT("analysis disabled");
                    }
                else if (vsCmd != NULL && vsCmd->equals("delta"))
                    {
                    // compare current against the heap mark
                    OUT((NULL == vSnap
                            ? vAnalyzer->capture() : vAnalyzer->delta(vSnap))
                           );

                    // reset to mark based on the current heap useage
                    vSnap  = NULL;
                    vSnap  = vAnalyzer->capture();
                    fDelta = true;
                    }
                else
                    {
                    // output the current heap useage
                    OUT(vAnalyzer);
                    }

                if (hCache != NULL)
                    {
                    size64_t lcb = hCache->sizeOf(/*fDeep*/ true);

                    if (fDelta)
                        {
                        if (lcb < lcbCacheSnap)
                            {
                            OUT(hCache->getCacheName() << " local heap usage delta: -"<<
                                    StringHelper::toMemorySizeString(
                                            lcbCacheSnap - lcb,
                                            /*fExact*/ false, /*fVerbose*/ true));
                            }
                        else
                            {
                            OUT(hCache->getCacheName() << " local heap usage delta: "<<
                                    StringHelper::toMemorySizeString(
                                            lcb - lcbCacheSnap,
                                            /*fExact*/ false, /*fVerbose*/ true));
                            }

                        lcbCacheSnap = lcb;
                        }
                    else
                        {
                        OUT(hCache->getCacheName() << " local heap usage: "<<
                                StringHelper::toMemorySizeString(lcb,
                                        /*fExact*/ false, /*fVerbose*/ true));
                        }
                    }
                }
            else if (vsCmd->equals("pof"))
                {
                // output the SystemPofContext
                OUT(SystemPofContext::getInstance());
                }
            else if (vsCmd->equals("put"))
                {
                // perform a put operation on the current cache
                Object::View vKey   = getNext(ssInput);
                Object::View vValue = getNext(ssInput);
                Object::View vPrev  = hCache->put(vKey, vValue);

                // print the old value
                OUT(vPrev);
                }
            else if (vsCmd->equals("remove"))
                {
                // perform a remove operation on the current cache
                Object::View vKey  = getNext(ssInput);
                Object::View vPrev = hCache->remove(vKey);

                // print the removed value
                OUT(vPrev);
                }
            else if (vsCmd->equals("release"))
                {
                // release the current cache
                CacheFactory::releaseCache(hCache);
                }
            else if (vsCmd->equals("size"))
                {
                // print the size of the current cache
                size32_t cElements = hCache->size();

                OUT(cElements);
                }
            else if (vsCmd->equals("shutdown"))
                {
                // shutdown coherence
                hCache = NULL;
                CacheFactory::shutdown();
                }
            else if (vsCmd->equals("threads"))
                {
                // print a stack trace for all threads related to coherence
                std::cout << Thread::formatStacks();
                }
            else if (vsCmd->equals(""))
                {
                continue;
                }
            else if (vsCmd->equals("help"))
                {
                // print help
                OUT("The commands are:"
                    << std::endl << "  batch <path>"
                    << std::endl << "  bye"
                    << std::endl << "  cache <name>"
                    << std::endl << "  classes"
                    << std::endl << "  clear"
                    << std::endl << "  destroy"
                    << std::endl << "  get <key>"
                    << std::endl << "  help"
                    << std::endl << "  list"
                    << std::endl << "  listen ('start' | 'stop') [<key>] [<lite>]"
                    << std::endl << "  load <library>"
                    << std::endl << "  memory [delta]"
                    << std::endl << "  pof"
                    << std::endl << "  put <key> <value>"
                    << std::endl << "  release"
                    << std::endl << "  remove <key>"
                    << std::endl << "  shutdown"
                    << std::endl << "  size"
                    << std::endl << "  threads"
                   );
                }
            else
                {
                OUT("Unknown command: \"" << vsCmd << "\"\n"
                    << "Entry \"help\" for command list");
                }
            }
        catch (NullPointerException::View e)
            {
            if (NULL == hCache)
                {
                OUT("Please specify a cache using the \"cache\" "
                          << "command.");
                }
            else
                {
                OUT(e);
                }
            }
        catch (Exception::View e)
            {
            OUT("Error: " << e);
            }
        }
    }

COH_CLOSE_NAMESPACE2
