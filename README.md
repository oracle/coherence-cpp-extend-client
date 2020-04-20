<!--

  Copyright (c) 2000, 2020, Oracle and/or its affiliates.

  Licensed under the Universal Permissive License v 1.0 as shown at
  http://oss.oracle.com/licenses/upl.

-->
-----
<img src=https://coherence.java.net/assets/img/logo-community.png><img>

[![License](http://img.shields.io/badge/license-UPL%201.0-blue.svg)](https://oss.oracle.com/licenses/upl/)

# Oracle Coherence for C++ Community Edition

## Contents
1. [Introduction to Coherence](#intro)
1. [How to Get Coherence Community Edition](#acquire)
1. [Introduction to Coherence for C++](#intro-cpp)
1. [Building](#build)
1. [CLI Hello Coherence Example](#cli-hello)
1. [Programmatic Hello Coherence Example](#prog-hello)
1. [Testing](#testing)
1. [Documentation](https://docs.oracle.com/en/middleware/fusion-middleware/coherence/12.2.1.4/index.html)
1. [Contributing](CONTRIBUTING.md)

# <a name="intro"></a>Introduction to Coherence

[Coherence](http://coherence.java.net/) is scalable, fault-tolerant, cloud-ready,
distributed platform for building grid-based applications and reliably storing data.
The product is used at scale, for both compute and raw storage, in a vast array of
industries including critical financial trading systems, high performance telecommunication
products and eCommerce applications to name but a few. Typically these deployments
do not tolerate any downtime and Coherence is chosen due its novel features in
death detection, application data evolvability, and the robust, battle-hardened
core of the product allowing it to be seamlessly deployed and adapt within any ecosystem.

At a high level, Coherence provides an implementation of the all too familiar Java `Map`
interface but rather than storing the associated data in the local process it is partitioned
(or sharded if you prefer) across a number of designated remote nodes. This allows
applications to not only distribute (and therefore scale) their storage across multiple
processes, machines, racks, and data centers but also to perform grid-based processing
to truly harness the cpu resources of the machines. The Coherence interface `NamedCache`
(an extension of `Map`) provides methods to query, aggregate (map/reduce style) and
compute (send functions to storage nodes for locally executed mutations) the data set.
These capabilities, in addition to numerous other features, allows Coherence to be used
as a framework to write robust, distributed applications.

# <a name="acquire"></a>How to Get Coherence Community Edition

For more details on how to obtain and use Coherence, please see the Coherence CE [README](https://github.com/oracle/coherence/README.md).

# <a name="intro_cpp"></a>Introduction to Coherence for C++

Coherence for C++ allows C++ applications to access Coherence clustered services, including data,
data events, and data processing from outside the Coherence cluster. Typical uses of Coherence
for C++ include desktop and web applications that require access to Coherence caches.

Coherence for C++ consists of a native C++ library that connects to a Coherence\*Extend clustered
service instance running within the Coherence cluster using a high performance TCP/IP-based
communication layer. This library sends all client requests to the Coherence\*Extend clustered
service which, in turn, responds to client requests by delegating to an actual Coherence
clustered service (for example, a partitioned or replicated cache service).

A NamedCache instance is retrieved by using the `CacheFactory::getCache(...)` API call.
After it is obtained, a client accesses the `NamedCache` in the same way as it would if it
were part of the Coherence cluster. The fact that `NamedCache` operations are being sent to a
remote cluster node (over TCP/IP) is completely transparent to the client application.

> **_NOTE:_** The C++ client follows the interface and concepts of the Java client, and
> users familiar with Coherence for Java should find migrating to Coherence for C++ straightforward.

See [Developing Remote Clients for Oracle Coherence](https://docs.oracle.com/en/middleware/fusion-middleware/coherence/12.2.1.4/develop-remote-clients/creating-c-extend-clients.html)
for further details.

# <a name="build"></a>Building

## Prerequisites

1. A supported hardware platform and C++ compiler. See [Supported Environments for Coherence C++ Client](https://docs.oracle.com/en/middleware/fusion-middleware/coherence/12.2.1.4/install/installing-client-distribution.html#GUID-49C698DD-D34A-416D-A71C-566C32824CEA).
1. Oracle Java 8 JDK
1. Apache Ant version 1.7.0 or later
1. [Ant-Contrib](http://ant-contrib.sourceforge.net/) version 1.0b3
1. Ant-Contrib [cpptasks](http://ant-contrib.sourceforge.net/cpptasks/index.html) version 1.0b4

## Building the shared library

The Coherence for C++ build system is based on Ant. To build Coherence for C++:

1. Clone this repository
1. Download Apache Ant version 1.7.0 or later and install it at `tools/internal/common/ant`
1. Download ant-contrib-1.0b3.jar and install it at `tools/internal/common/ant-contrib/lib`
1. Download cpptasks-1.0b4.jar and install it at `tools/internal/common/ant-contrib/lib`
1. Set the `JAVA_HOME` environment variable to point to the Oracle JDK 8 home
1. `cd bin`
1. On unix flavor platforms source `cfglocal.sh` from a bash shell (e.g. `. cfglocal.sh`).  On windows open a Visual Studio native tools command prompt and run `cfgwindows.cmd`
1. `cd ../prj/coherence`
1. `ant -Dbuild.type=release clean build dist` - can take from 20 minutes to over an hour depending on platform type and CPU speed

The resulting files:

* `dist/14.1.2.0.0b0/include` - the public header files
* `dist/14.1.2.0.0b0/lib` - the Coherence for C++ shared library

# <a name="cli-hello"></a>CLI Hello Coherence Example

The following example illustrates starting a **storage enabled** Coherence Server,
followed by a Coherence for C++ console application. Using the console, data is
inserted and retrieved. The console is then terminated and restarted and data is once again
retrieved to illustrate the permanence of the data.

## Prerequisites

1. The Coherence for C++ shared library see [Building](#build)
1. [Coherence Community Edition](https://github.com/oracle/coherence) coherence.jar
  
## Build Sanka

Sanka is a command line tool which can be used to run the `main` method on Coherence C++ classes

1. Set the `JAVA_HOME` environment variable to point to the Oracle JDK 8 home
1. `cd bin`
1. On unix flavor platforms source `cfglocal.sh` from a bash shell (e.g. `. cfglocal.sh`).  On windows open a Visual Studio native tools command prompt and run `cfgwindows.cmd`
1. `cd ../prj/sanka`
1. `ant -Dbuild.type=release` build dist

## Start a Coherence server

Unix shell:

```shell script
$JAVA_HOME/bin/java -Dcoherence.pof.enabled=true -Dcoherence.log.level=6 -jar coherence.jar
```

Windows command prompt:

```shell script
"%JAVA_HOME%/bin/java" -coherence.pof.enabled=true -Dcoherence.log.level=6 -jar coherence.jar
```

## Run the `coherence::net::CacheFactory::main()` (console) using sanka

Unix shell:

```shell script
cd dist/14.1.2.0.0b0/bin
./sanka -l ../lib/libcoherence.so coherence::net::CacheFactory
````

Windows command prompt:

```shell script
.\sanka -l ..\lib\coherence.dll coherence::net::CacheFactory
```

Console:

```shell script
Map (?): cache welcomes

Map (welcomes): get english
NULL

Map (welcomes): put english Hello
NULL

Map (welcomes): put spanish Hola
NULL

Map (welcomes): put french Bonjour
NULL

Map (welcomes): get english
Hello

Map (welcomes): list
french = Bonjour
english = Hello
spanish = Hola

Map (welcomes): bye
```

Start the console again

Unix shell:

```shell script
./sanka -l ../lib/libcoherence.so coherence::net::CacheFactory
```

Windows command prompt:

```shell script
.\sanka -l ..\lib\coherence.dll coherence::net::CacheFactory
```

Console:

```shell script
Map (?): cache welcomes

Map (welcomes): list
french = Bonjour
english = Hello
spanish = Hola
```

# <a name="prog-hello"></a>Programmatic Hello Coherence Example

The following example illustrates starting a **storage enabled** Coherence server,
followed by running the `HelloCoherence` application. The `HelloCoherence` application
inserts and retrieves data from the Coherence server.

## Prerequisites

1. The Coherence for C++ shared library see [Building](#build)
1. [Coherence Community Edition](https://github.com/oracle/coherence) coherence.jar

## Build `HelloCoherence`

Copy and paste the following source to a file named `HelloCoherence.cpp`:

```c++
#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Iterator.hpp"

#include <iostream>

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::Iterator;

int main()
    {
    try
        {
        // access/create the "welcomes" cache in the Coherence cluster
        NamedCache::Handle hCache = CacheFactory::getCache("welcomes");
        std::cout << "Accessing cache \"" << hCache->getCacheName()
                  << "\" containing " << hCache->size() << " entries"
                  << std::endl;

        hCache->put(String::create("english"), String::create("Hello"));
        hCache->put(String::create("spanish"), String::create("Hola"));
        hCache->put(String::create("french"), String::create("Bonjour"));

        // list
        for (Iterator::Handle hIterator = hCache->entrySet()->iterator();
             hIterator->hasNext(); )
            {
            std::cout << hIterator->next() << std::endl;
            }

        // disconnect from the Coherence cluster
        CacheFactory::shutdown();
        }
    catch (const std::exception& e)
        {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
        }
    return 0;
    }
```

Compile `HelloCoherence.cpp` using the Coherence for C++ header files and shared library:

Using GCC on Linux and macOS:

```shell script
g++ -Idist/14.1.2.0.0b0/include -lcoherence -L dist/14.1.2.0.0b0/lib -o HelloCoherence HelloCoherence.cpp
```

From a Visual Studio native tools command prompt on Windows:

```shell script
cl -Idist\14.1.2.0.0b0\include dist\14.1.2.0.0b0\lib\coherence.lib -o HelloCoherence HelloCoherence.cpp
```

## Start a Coherence server

Unix shell:

```shell script
$JAVA_HOME/bin/java -Dcoherence.pof.enabled=true -Dcoherence.log.level=6 -jar coherence.jar
```

Windows command prompt:

```shell script
"%JAVA_HOME%/bin/java" -coherence.pof.enabled=true -Dcoherence.log.level=6 -jar coherence.jar
```

## Run `HelloCoherence`

Unix shell:

```shell script
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:dist/14.1.2.0.0b0/lib
./HelloCoherence
```

Windows command prompt:

```shell script
set PATH=%DEV_ROOT%\dist\14.1.2.0.0b0\lib;%PATH%
HelloCoherence
```

# <a name="testing"></a>Testing

Coherence for C++ has unit and functional tests to validate builds and code changes.

## Prerequisites

1. The Coherence for C++ shared library see [Building](#build)
1. [Perl](https://www.perl.org/)
1. Download [CxxTest](https://cxxtest.com/) version 3.10.1 and install it at `tools/internal/common/cxxtest`
1. Comment out the `-Werror` flag in `prj/build-import.xml`
(e.g. `<!-- compilerarg if="cc.g++" value="-Werror"/ -->`) as CxxTest source
files will produce warnings when compiling with GCC

## Building and running the tests

1. Set the `JAVA_HOME` environment variable to point to the Oracle JDK 8 home
1. `cd bin`
1. On unix flavor platforms source `cfglocal.sh` from a bash shell (e.g. `. cfglocal.sh`).  On windows open a Visual Studio native tools command prompt and run `cfgwindows.cmd`

```shell script
cd ../prj/tests/unit (or cd ../prj/tests/functional)
ant -Dbuild.type=release build test
```

