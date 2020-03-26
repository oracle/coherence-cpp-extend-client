#!/bin/bash
#
#  Copyright (c) 2000, 2020, Oracle and/or its affiliates.
#
#  Licensed under the Universal Permissive License v 1.0 as shown at
#  http://oss.oracle.com/licenses/upl.
#
java -Dtangosol.coherence.extend.address=127.0.0.1 -Dtangosol.coherence.localhost=127.0.0.1 -Dtangosol.coherence.clusterport=18943 -Dtangosol.coherence.ttl=0 -Dtangosol.coherence.cacheconfig=../../../examples/config/extend-server-config.xml -cp ../../../lib/common/coherence/coherence.jar:lib/cluster-control.jar com.tangosol.net.DefaultCacheServer
