package es.tid.cosmos.platform.injection.server;

import java.net.URI;
import java.net.URL;

import org.apache.commons.configuration.ConfigurationException;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 * @author sortega
 */
public class ConfigurationTest {
    @Test(expected = ConfigurationException.class)
    public void shouldThrowExceptionWhenNotFound() throws Exception {
        new Configuration(getClass().getResource("/not/existing"));
    }

    @Test
    public void shouldParseProperties() throws Exception {
        URL testConfig = getClass().getResource("test_config.properties");
        Configuration instance = new Configuration(testConfig);

        assertEquals(2222, instance.getPort());
        assertEquals(URI.create("hdfs://pshdp01:8011"), instance.getHdfsUrl());
        assertEquals("pshdp01:8012", instance.getJobTrackerUrl());
        assertEquals("jdbc:mysql:localhost", instance.getFrontendDbUrl());
        assertEquals("database", instance.getDbName());
        assertEquals("root", instance.getDbUser());
        assertEquals("toor", instance.getDbPassword());
    }
}
