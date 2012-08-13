/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.base.test.sampleutilities;

/**
 * Sample invalid utility class for testing UtilityClassTest
 *
 * Does significant work in the constructor.
 *
 * @author sortega
 */
public final class BusyConstructorUtilityClass {

    private BusyConstructorUtilityClass() throws Exception {
        throw new Exception("something that can throw exceptions");
    }

    public static BusyConstructorUtilityClass sampleMethod(int in)
            throws Exception {
        return new BusyConstructorUtilityClass();
    }
}
