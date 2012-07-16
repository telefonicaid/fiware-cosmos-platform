package es.tid.cosmos.tests.environment;

import java.io.IOException;
import java.util.Properties;

import static org.testng.Assert.fail;

/**
 *
 * @author ximo
 */
public enum Environment {
    Test(Environment.TEST_CONFIG_FILE),
    Production(Environment.PRODUCTION_CONFIG_FILE);

    private static final String TEST_CONFIG_FILE = "/test.properties";
    private static final String PRODUCTION_CONFIG_FILE
        = "/production.properties";

    private Properties properties;

    Environment(String configFilePath) {
        this.properties = new Properties();
        try {
            properties.load(Environment.class.getResource(configFilePath)
                    .openStream());
        } catch (IOException ex) {
            fail("IOException while loading configuration: " + ex.toString());
        }
    }

    public String getProperty(EnvironmentSetting setting) {
        return this.properties.getProperty(setting.toString());
    }
}
