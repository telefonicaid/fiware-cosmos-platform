
package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * @author ximo
 */
public abstract class ClientProfileUtil {
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
    
    public static ProtobufWritable createAndWrap(long userId, int profileId) {
        return wrap(create(userId, profileId));
    }
}
