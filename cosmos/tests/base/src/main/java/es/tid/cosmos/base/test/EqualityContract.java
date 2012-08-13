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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import static org.junit.Assert.*;

/**
 * Checks the behaviour of equals and hashCode.
 *
 * @author sortega
 */
public class EqualityContract<T> {

    private final List<Collection<T>> classes;

    /**
     * Creates an equality contract checker given two or more equivalence
     * classes.
     *
     * Elements from the same equivalence class should be equal and have the
     * same hash code.
     *
     * @param class1
     * @param class2
     * @param otherClasses
     */
    public EqualityContract(Collection<T> class1, Collection<T> class2,
                            Collection<T>... otherClasses) {
        this.classes = new ArrayList<Collection<T>>();
        this.classes.add(class1);
        this.classes.add(class2);
        this.classes.addAll(Arrays.asList(otherClasses));
    }

    /**
     * Checks the contract.
     *
     * Intended to be used as part of a JUnit test.
     */
    public void verify() {
        this.verifySameHashCodePerClass();
        this.verifyEqualityPerClass();
        this.verifyInterClassInequality();
        this.verifyNotEqualNull();
    }

    private void verifySameHashCodePerClass() {
        for (Collection<T> elements : this.classes) {
            final T prototype = elements.iterator().next();
            int prototypeHashCode = prototype.hashCode();
            for (T element : elements) {
                final String message = String.format(
                        "Hash code mismatch: '%s' has %d while '%s' has %d",
                        prototype, prototypeHashCode, element, element.hashCode());
                assertEquals(message, prototypeHashCode, element.hashCode());
            }
        }
    }

    private void verifyEqualityPerClass() {
        for (Collection<T> elements : this.classes) {
            for (T leftElement : elements) {
                for (T rightElement : elements) {
                    assertEquals("Not equal elements from the same class",
                                 leftElement, rightElement);
                }
            }
        }
    }

    private void verifyInterClassInequality() {
        for (Collection<T> leftClass : this.classes) {
            for (Collection<T> rightClass : this.classes) {
                if (leftClass.equals(rightClass)) {
                    continue;
                }
                for (T leftElement : leftClass) {
                    for (T rightElement : rightClass) {
                        final String message = String.format("Elements "
                                + "of different classes '%s' and '%s' must "
                                + "be different", leftElement, rightElement);
                        assertFalse(message, leftElement.equals(rightElement));
                    }
                }
            }
        }
    }

    private void verifyNotEqualNull() {
        for (Collection<T> elements : this.classes) {
            for (T element : elements) {
                assertFalse("Should be not equalt to null",
                            element.equals(null)); // NOPMD
            }
        }
    }
}
