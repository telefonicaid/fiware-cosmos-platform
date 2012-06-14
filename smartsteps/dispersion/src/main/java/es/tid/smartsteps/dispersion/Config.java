package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;

/**
 *
 * @author dmicol
 */
public abstract class Config {
    public static final String DELIMITER = "delimiter";
    public static final String DATE_TO_FILTER = "date_to_filter";
    
    private Config() {
    }
    
    public static Configuration load(InputStream configInput,
            final Configuration configuration) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        
        Configuration conf = new Configuration(configuration);
        conf.set(DELIMITER, props.getProperty(DELIMITER));
        conf.set(DATE_TO_FILTER, props.getProperty(DATE_TO_FILTER));
        
        return conf;
    }
}
