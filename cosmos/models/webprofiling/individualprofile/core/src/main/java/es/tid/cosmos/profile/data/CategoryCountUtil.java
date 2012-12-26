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

package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.profile.generated.data.ProfileProtocol;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Utils for CategoryCount
 *
 * @author sortega
 */
public final class CategoryCountUtil {
    private static final String DELIMITER = "\t";

    private CategoryCountUtil() {
    }

    public static CategoryCount create(String name, long count) {
        return ProfileProtocol.CategoryCount
                .newBuilder()
                .setName(name)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<CategoryCount> createAndWrap(String name,
            long count) {
        ProtobufWritable<CategoryCount> wrapper =
                ProtobufWritable.newInstance(CategoryCount.class);
        wrapper.set(create(name, count));
        return wrapper;
    }

    public static String toString(CategoryCount obj) {
        return (obj.getName() + DELIMITER + obj.getCount());
    }
}
