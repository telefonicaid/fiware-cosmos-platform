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

import static java.util.Arrays.asList;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

/**
 *
 * @author sortega
 */
public class EqualityContractTest {
    @Rule
    public ExpectedException exception = ExpectedException.none();

    @Test
    public void testValidEquality() {
        EqualityContract<String> instance = new EqualityContract<String>(
                asList("hello", "hello"), asList("bye"));
        instance.verify();
    }

    @Test
    public void testInvalidEquality() {
        EqualityContract<WrongClass> instance =
                new EqualityContract<WrongClass>(asList(new WrongClass(1, 2),
                                                        new WrongClass(1, 2)),
                                                 asList(new WrongClass(1, 4)));
        exception.expectMessage("Elements of different classes '(a=1, b=2)' "
                + "and '(a=1, b=4)' must be different");
        instance.verify();
    }

    private static class WrongClass {
        private final int a, b;

        public WrongClass(int a, int b) {
            this.a = a;
            this.b = b;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null) {
                return false;
            }
            if (getClass() != obj.getClass()) {
                return false;
            }
            final WrongClass other = (WrongClass) obj;
            if (this.a != other.a) {
                return false;
            }
            return true;
        }

        @Override
        public int hashCode() {
            return 97 * 7 + this.b;
        }

        @Override
        public String toString() {
            return "(" + "a=" + a + ", b=" + b + ')';
        }
    }
}
