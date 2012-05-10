package es.tid.cosmos.mobility.data;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;

/**
 *
 * @author dmicol
 */
public class MobViMobVarsUtilTest {
    @Test
    public void testCreateAndWrap() {
        MobVars mobVars1 = MobVarsUtil.create(1, true, 2, 3, 4, 5, 6, 7, 8, 9);
        MobVars mobVars2 = MobVarsUtil.create(10, false, 20, 30, 40, 50, 60, 70,
                80, 90);
        ProtobufWritable<MobViMobVars> wrapper = MobViMobVarsUtil.createAndWrap(
                asList(mobVars1, mobVars2));
        wrapper.setConverter(MobViMobVars.class);
        final MobViMobVars obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(mobVars1, obj.getVars(0));
        assertEquals(mobVars2, obj.getVars(1));
    }
}
