package es.tid.smartsteps.dispersion.config;

import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;

/**
 *
 * @author dmicol
 */
public abstract class Config {

    public static final String DELIMITER = "delimiter";
    public static final String DATES_TO_FILTER = "dates_to_filter";
    public static final String COUNT_FIELDS = "count_fields";
    public static final String ROUND_RESULTS = "round_results";
    
    private Config() {
    }
    
    public static Configuration load(InputStream configInput,
            final Configuration configuration) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        
        Configuration conf = new Configuration(configuration);
        conf.set(DELIMITER, props.getProperty(DELIMITER));
        conf.setStrings(DATES_TO_FILTER,
                        getMultiValueProperty(props, DATES_TO_FILTER));

        conf.setStrings(COUNT_FIELDS,
                        getMultiValueProperty(props, COUNT_FIELDS));
        conf.setBoolean(ROUND_RESULTS,
                props.getProperty(ROUND_RESULTS).equalsIgnoreCase("true") ?
                        true : false);
        
        return conf;
    }
    
    private static String[] getMultiValueProperty(Properties props,
                                                  String name) {
        List<String> values = new LinkedList<String>();
        for (String field : props.getProperty(name).split(",")) {
            values.add(field.trim().replaceAll("\"", ""));
        }
        return values.toArray(new String[values.size()]);
    }
}
