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
 * It has a public constructor.
 *
 * @author sortega
 */
public final class PublicConstructorUtilityClass {
    private final int i;

    public PublicConstructorUtilityClass(int i) {
        this.i = i;
    }

    public static int sampleMethod(int in) {
        return 2 * in;
    }
}
