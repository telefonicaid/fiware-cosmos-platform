
package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * @author ximo
 */
public class ClientProfileUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ClientProfile> wrapper =
                ClientProfileUtil.createAndWrap(1L, 2);
        wrapper.setConverter(ClientProfile.class);
        ClientProfile obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getUserId());
        assertEquals(2, obj.getProfileId());
    }
}
