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

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * @author ximo
 */
public final class ClientProfileUtil {

    private ClientProfileUtil() {}

    public static ClientProfile create(long userId, int profileId) {
        return ClientProfile.newBuilder()
                .setUserId(userId)
                .setProfileId(profileId)
                .build();
    }

    public static ProtobufWritable<ClientProfile> wrap(ClientProfile obj) {
        ProtobufWritable<ClientProfile> wrapper = ProtobufWritable.newInstance(
                ClientProfile.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ClientProfile> createAndWrap(
            long userId, int profileId) {
        return wrap(create(userId, profileId));
    }
}
