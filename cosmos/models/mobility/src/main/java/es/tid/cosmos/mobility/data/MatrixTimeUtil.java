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
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;

/**
 *
 * @author dmicol
 */
public final class MatrixTimeUtil {

    private MatrixTimeUtil() {}

    public static MatrixTime create(Date date, Time time, int group, long bts) {
        return MatrixTime.newBuilder()
                .setDate(date)
                .setTime(time)
                .setGroup(group)
                .setBts(bts)
                .build();
    }

    public static ProtobufWritable<MatrixTime> wrap(MatrixTime obj) {
        ProtobufWritable<MatrixTime> wrapper = ProtobufWritable.newInstance(
                MatrixTime.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MatrixTime> createAndWrap(Date date,
            Time time, int group, long bts) {
        return wrap(create(date, time, group, bts));
    }
}
