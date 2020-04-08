/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
package com.tangosol.tests.net.cache;

import com.tangosol.net.AbstractInvocable;
import com.tangosol.net.CacheFactory;

import com.tangosol.util.Base;

/**
* Invocable to shutdown all instances of the cluster.
*
* @author NSA 2008.05.12
*/
public class ShutdownAll extends AbstractInvocable
    {
    public static class ShutdownThread implements Runnable
        {
        public void run()
            {
            CacheFactory.log("ShutdownAll invocable exiting the JVM in 3 seconds...", Base.LOG_WARN);
            try
                {
                // sleep to allow the invocable to return
                Thread.sleep(3000);
                }
            catch (Throwable oops)
                {
                oops.printStackTrace();
                }
            finally
                {
                Runtime.getRuntime().halt(0);
                }
            }
        }

    public void run()
        {
        (new Thread(new ShutdownThread())).start();
        }
    }
