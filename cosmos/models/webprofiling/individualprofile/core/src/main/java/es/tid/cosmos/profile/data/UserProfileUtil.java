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

import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Utils for UserProfile class.
 *
 * @author dmicol
 */
public final class UserProfileUtil {
    private static final String DELIMITER = "\t";

    private UserProfileUtil() {
    }

    public static UserProfile create(String userId, String date,
                                     Iterable<CategoryCount> counts) {
        return UserProfile.newBuilder()
                .setUserId(userId)
                .setDate(date)
                .addAllCounts(counts)
                .build();
    }

    public static ProtobufWritable<UserProfile> createAndWrap(String userId,
            String date, Iterable<CategoryCount> counts) {
        ProtobufWritable<UserProfile> wrapper = ProtobufWritable.newInstance(
                UserProfile.class);
        wrapper.set(create(userId, date, counts));
        return wrapper;
    }

    public static String toString(UserProfile obj) {
        String str = obj.getUserId() + DELIMITER + obj.getDate();
        for (CategoryCount count : obj.getCountsList()) {
            str += DELIMITER + CategoryCountUtil.toString(count);
        }
        return str;
    }
}
