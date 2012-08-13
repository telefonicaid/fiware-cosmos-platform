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

package es.tid.cosmos.base.test;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import es.tid.cosmos.base.test.sampleutilities.*;
import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;

/**
 * @author sortega
 */
public class UtilityClassTestTest {
    @Rule
    public ExpectedException exception = ExpectedException.none();

    @Test
    public void testValidUtilityClass() {
        assertUtilityClass(ValidUtilityClass.class);
        assertUtilityClass(UtilityClassTest.class);
    }

    @Test
    public void testNotFinalUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should be final");
        assertUtilityClass(NotFinalUtilityClass.class);
    }

    @Test
    public void testPublicConstructorUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should not have " +
                                     "public constructors");
        assertUtilityClass(PublicConstructorUtilityClass.class);
    }

    @Test
    public void testNonStaticUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class should have static " +
                                     "methods only");
        assertUtilityClass(NonStaticUtilityClass.class);
    }

    @Test
    public void testInvalidConstructorUtilityClass() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class constructor should have " +
                                     "no arguments");
        assertUtilityClass(InvalidConstructorUtilityClass.class);
    }

    @Test
    public void testInvalidConstructorUtilityClass2() {
        this.exception.expect(AssertionError.class);
        this.exception.expectMessage("Utility class constructor should not " +
                                     "throw exceptions");
        assertUtilityClass(BusyConstructorUtilityClass.class);
    }
}
