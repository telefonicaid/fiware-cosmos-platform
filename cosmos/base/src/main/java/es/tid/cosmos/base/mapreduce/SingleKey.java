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

package es.tid.cosmos.base.mapreduce;

/**
 * The single key type for mapreduce applications.
 *
 * @author javierb
 */
public class SingleKey extends CompositeKey {
    private static final int CAPACITY = 1;

    private enum KeyIndex {
        PRIMARY
    }

    public SingleKey() {
        super(CAPACITY);
    }

    public SingleKey(String k1) {
        super(CAPACITY);
        this.set(KeyIndex.PRIMARY.ordinal(), k1);
    }

    public String getKey() {
        return this.get(KeyIndex.PRIMARY.ordinal());
    }

    public void setKey(String key) {
        this.set(KeyIndex.PRIMARY.ordinal(), key);
    }
}
