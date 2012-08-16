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

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 *
 * @author dmicol
 */
public final class ItinTimeUtil {

    private ItinTimeUtil() {}

    public static ItinTime create(Date date, Time time, long bts) {
        return ItinTime.newBuilder()
                .setDate(date)
                .setTime(time)
                .setBts(bts)
                .build();
    }

    public static ProtobufWritable<ItinTime> wrap(ItinTime obj) {
        ProtobufWritable<ItinTime> wrapper = ProtobufWritable.newInstance(
                ItinTime.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ItinTime> createAndWrap(Date date, Time time,
                                                           long bts) {
        return wrap(create(date, time, bts));
    }
}
