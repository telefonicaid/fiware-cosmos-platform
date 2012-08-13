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

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

import static org.junit.Assert.*;

/**
 * Utility class for testing utility classes.
 *
 * @author sortega
 */
public final class UtilityClassTest {
    private UtilityClassTest() {
        // Utility class
    }

    /**
     * Assert that the passed class is final, has a zero-args, non-public
     * constructor and all its methods are static.
     *
     * Meant to be used as part or a JUnit test.
     *
     * @param clazz
     */
    public static void assertUtilityClass(Class clazz) {
        assertTrue("Utility class should be final",
                   Modifier.isFinal(clazz.getModifiers()));

        for (Constructor constructor : clazz.getDeclaredConstructors()) {
            if (Modifier.isPublic(constructor.getModifiers())) {
                fail("Utility class should not have public constructors");
            } else {
                assertValidConstructor(constructor);
            }
        }

        for (Method method : clazz.getDeclaredMethods()) {
            if (!Modifier.isStatic(method.getModifiers())) {
                fail("Utility class should have static methods only");
            }
        }
    }

    /**
     * Check private constructor arguments and exercise it.
     *
     * @param constructor
     */
    private static void assertValidConstructor(Constructor constructor) {
        assertEquals("Utility class constructor should have no arguments",
                     0, constructor.getParameterTypes().length);
        constructor.setAccessible(true);
        try {
            constructor.newInstance();
        } catch (Exception ex) {
            fail("Utility class constructor should not throw exceptions");
        }
    }
}
