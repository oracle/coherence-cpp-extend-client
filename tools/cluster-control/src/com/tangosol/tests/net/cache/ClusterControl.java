/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
 package com.tangosol.tests.net.cache;

import com.tangosol.net.CacheFactory;
import com.tangosol.net.Cluster;
import com.tangosol.net.InvocationService;
import com.tangosol.net.Service;

import com.tangosol.util.Base;

import java.util.Set;

/**
* Programatically control a cluster for use with C++ functional tests.
*
* @author NSA 2008.05.12
*/
public class ClusterControl extends Base
    {
    /**
    * Shutdown the grid using the ShutdownAll invocable
    */
    public static void shutdownGrid()
        {
        try
            {
            InvocationService s = (InvocationService) CacheFactory.getService("InvocationService");

            Set setMembers = s.getInfo().getServiceMembers();
            setMembers.remove(CacheFactory.getCluster().getLocalMember());

            CacheFactory.log("Members to shut down: " + setMembers);
            s.query(new ShutdownAll(), setMembers);

            // give the nodes a few seconds to kill themselves
            sleep(10000);
            CacheFactory.log("Members remaining: " + s.getInfo().getServiceMembers());
            }
        catch (Throwable oops)
            {
            oops.printStackTrace();
            }
        finally
            {
            exit(0);
            }
        }

    /**
    * Show the current usage of the ClusterControl main.
    */
    public static void showUsage()
        {
        System.out.println("ClusterControl arguments:");
        System.out.println("\tstop - Stop the running cluster.");
        System.out.println("\tensure [count] - count is 1 by default");
        }

    public static void ensureRunningCount(int c)
        {
        int cMembers = 0;

        try
            {
            int     cExpected     = c + 1;
            boolean fServiceFound = false;

            Cluster cluster = CacheFactory.ensureCluster();

            for (int pingAttempts = 30, a = 1; !(cMembers == cExpected && fServiceFound) && a <= pingAttempts; ++a)
                {
                cMembers = cluster.getMemberSet().size();

                try
                    {
                    Service svc = CacheFactory.getService("ExtendTcpCacheService");
                    fServiceFound = true;
                    }
                catch(Throwable t)
                    {
                    if (a == pingAttempts)
                        {
                        CacheFactory.log("***ClusterControl.ensure - final attempt at getService failed with: " + t.getMessage(),
                                Base.LOG_ERR);
                        }
                    fServiceFound = false;
                    }

                if (cMembers != cExpected || !fServiceFound)
                    {
                    CacheFactory.log("***ClusterControl.ensure - Found "
                        + (cMembers - 1) + " of " + c
                        + " expected members running.  ExtendTcpCacheService "
                        + (fServiceFound ? " " : "NOT")
                        + " found. Attempt " + a + " of " + pingAttempts);
                    sleep(1000);
                    }
                }

            if (cMembers != cExpected)
                {
                CacheFactory.log("***ClusterControl.ensure - Found "
                        + (cMembers - 1) + " of " + c
                        + " expected members running. Exiting with error***", Base.LOG_ERR);
                exit(-1);
                }

            if (!fServiceFound)
                {
                CacheFactory.log("***ClusterControl.ensure - Could not find the proxy service running. Exiting with error***",
                        Base.LOG_ERR);
                exit(-1);
                }
            }
        catch(Throwable oops)
            {
            oops.printStackTrace();
            exit(-1);
            }

        CacheFactory.log("***ClusterControl.ensure - Found "
                + (cMembers - 1) + " out of " + c + " members running***");

        exit(0);
        }

    private static void exit(int nCode)
        {
        CacheFactory.shutdown();

        if (nCode == 0)
            {
            // give the nodes a few seconds to complete starting their services before returning
            sleep(5000);
            }

        System.exit(nCode);
        }

    public static void main(String[] asArg)
        {
        String sConfigPath = System.getProperty("tangosol.coherence.cacheconfig");
        System.out.println("CacheConfig: " + sConfigPath);

        for (int x = 0, c = asArg.length; x < c; ++x)
            {
            if (asArg[x].compareToIgnoreCase("stop") == 0)
                {
                shutdownGrid();
                }
            else if (asArg[x].compareToIgnoreCase("ensure") == 0)
                {
                if (x + 1 < c)
                    {
                    ensureRunningCount(Integer.parseInt(asArg[++x]));
                    }
                else
                    {
                    ensureRunningCount(1);
                    }
                }
            else
                {
                showUsage();
                }
            }
        }
    }
