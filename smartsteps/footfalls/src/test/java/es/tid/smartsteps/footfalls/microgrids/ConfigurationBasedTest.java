package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;

import es.tid.smartsteps.footfalls.microgrids.config.Config;

/**
 *
 * @author dmicol
 */
public class ConfigurationBasedTest {

    protected final Configuration conf;

    public ConfigurationBasedTest() throws IOException {
        this.conf = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                new Configuration());
    }
}
