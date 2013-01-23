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

package es.tid.cosmos.kpicalculation;

import org.apache.hadoop.io.DataOutputBuffer;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * @author sortega
 */
public class BinaryComparatorBaseTest {
    protected byte[] toByteArray(CompositeKey key) throws Exception {
        DataOutputBuffer out = new DataOutputBuffer();
        key.write(out);
        return out.getData();
    }
}
