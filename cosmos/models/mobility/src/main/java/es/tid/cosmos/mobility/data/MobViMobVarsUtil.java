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
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

import java.util.List;

/**
 *
 * @author losa
 */
public final class MobViMobVarsUtil {

    private MobViMobVarsUtil() {}

    public static MobViMobVars create(Iterable<MobVars> areas) {
        MobViMobVars.Builder repActAreas = MobViMobVars.newBuilder()
                .addAllVars(areas);
        return repActAreas.build();
    }

    public static ProtobufWritable<MobViMobVars> wrap(MobViMobVars obj) {
        ProtobufWritable<MobViMobVars> wrapper =
            ProtobufWritable.newInstance(MobViMobVars.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobViMobVars> createAndWrap(
            Iterable<MobVars> allAreas) {
        return wrap(create(allAreas));
    }

    public static String toString(MobViMobVars obj, String separator) {
        List<MobVars> allAreas = obj.getVarsList();
        String ans = "";
        int pos = 0;
        for (MobVars area : allAreas) {
            pos += 1;
            ans += MobVarsUtil.toString(area, separator);
            if (pos < allAreas.size()) {
                ans += separator;
            }
        }
        return ans;
    }

}
