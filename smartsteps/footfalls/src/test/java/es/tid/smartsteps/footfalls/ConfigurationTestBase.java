package es.tid.smartsteps.footfalls;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;

import es.tid.smartsteps.footfalls.config.Config;

/**
 *
 * @author dmicol
 */
public class ConfigurationTestBase {

    protected final Configuration conf;

    public ConfigurationTestBase() {
        try {
            this.conf = Config.load(
                    Config.class.getResource("/config.properties")
                    .openStream(),
                    new Configuration());
        } catch (IOException ex) {
            throw new ExceptionInInitializerError(ex);
        }
    }
}
