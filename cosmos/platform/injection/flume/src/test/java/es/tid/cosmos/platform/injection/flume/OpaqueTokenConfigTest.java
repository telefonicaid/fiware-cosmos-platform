package es.tid.cosmos.platform.injection.flume;

import org.junit.Before;
import org.junit.Test;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotNull;

/**
 * Unit test for opaque token config.
 *
 * @author apv
 */
public class OpaqueTokenConfigTest {

    @Before
    public void setUp() throws Exception {
    }

    @Test
    public void testBuildNewConfig() {
        OpaqueTokenConfig config = OpaqueTokenConfig.newBuilder()
                .withTransformation("fakeTransformation")
                .withDestinationPath("/home/apv/mystuff")
                .build();
        assertNotNull(config);
        assertEquals("fakeTransformation", config.getTransformation());
        assertEquals("/home/apv/mystuff", config.getDestinationPath());
    }

    @Test(expected = IllegalStateException.class)
    public void testBuildNotCompletedConfig() {
        OpaqueTokenConfig.newBuilder()
                .withTransformation("fakeTransformation")
                .build(); // missing mandatory field 'destinationPath'
    }
}
