/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

public class AssignmentTest
    {
    /**
    * Test entry point
    */
    public static void main(String[] asArg)
        {
        int cAssign = asArg.length > 0
            ? Integer.parseInt(asArg[0])
            : 10000;

        Object o        = new Object();
        long   ldtStart = System.currentTimeMillis();

        for (int i = 0; i < cAssign; ++i)
            {
            Object o2 = o;
            }

        long ldtEnd  = System.currentTimeMillis();
        long cMillis = ldtEnd - ldtStart;

        System.out.println("performed " + cAssign
            + " assignments in "
            + cMillis + " ms; throughput = "
            + (cAssign / (cMillis / 1000.0)) + "/sec");
        }
    }
