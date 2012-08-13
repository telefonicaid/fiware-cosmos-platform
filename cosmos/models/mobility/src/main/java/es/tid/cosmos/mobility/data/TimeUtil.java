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

package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;

/**
 *
 * @author sortega
 */
public final class TimeUtil {

    private TimeUtil() {}

    public static Time create(int hour, int minute, int seconds) {
        return Time.newBuilder()
                .setHour(hour)
                .setMinute(minute)
                .setSeconds(seconds)
                .build();
    }

    public static ProtobufWritable<Time> wrap(Time obj) {
        ProtobufWritable<Time> wrapper =
                ProtobufWritable.newInstance(Time.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Time> createAndWrap(int hour, int minute,
                                                       int seconds) {
        return wrap(create(hour, minute, seconds));
    }

    public static String toString(Time obj, String separator) {
        return (obj.getHour() + separator + obj.getMinute() + separator +
                obj.getSeconds());
    }
}
