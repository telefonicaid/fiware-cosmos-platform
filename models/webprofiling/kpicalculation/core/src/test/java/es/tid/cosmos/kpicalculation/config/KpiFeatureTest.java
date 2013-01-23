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

package es.tid.cosmos.kpicalculation.config;

import static java.util.Arrays.asList;

import org.junit.Test;

import es.tid.cosmos.base.test.EqualityContract;

/**
 * @author sortega
 */
public class KpiFeatureTest {

    @Test
    public void testEqualityContract() {
        KpiFeature a1 = new KpiFeature("a", new String[] {"f1", "f2"}),
                   a2 = new KpiFeature("a", new String[] {"f1", "f2"}, null),
                   b = new KpiFeature("a", new String[] {"f1", "f2"}, "group-b"),
                   c = new KpiFeature("c", new String[] {"f1"}, "group-b");

        new EqualityContract<KpiFeature>(asList(a1, a2), asList(b), asList(c))
                .verify();
    }
}
