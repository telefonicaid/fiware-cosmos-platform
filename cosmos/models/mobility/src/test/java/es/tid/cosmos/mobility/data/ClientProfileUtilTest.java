
package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author ximo
 */
public class ClientProfileUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ClientProfileUtil.class);
    }

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
