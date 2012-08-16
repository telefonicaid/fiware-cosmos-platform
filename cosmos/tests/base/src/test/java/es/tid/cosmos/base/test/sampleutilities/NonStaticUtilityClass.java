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
 * Sample invalid utility class for testing UtilityClassTest.
 *
 * It has non-static methods.
 *
 * @author sortega
 */
public final class NonStaticUtilityClass {

    private NonStaticUtilityClass() {
        // Utility class
    }

    public int sampleMethod(int in) {
        return 2 * in;
    }
}
