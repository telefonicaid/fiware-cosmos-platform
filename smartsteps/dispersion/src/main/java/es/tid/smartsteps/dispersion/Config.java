package es.tid.smartsteps.dispersion;

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
    public static final String DATE_TO_FILTER = "date_to_filter";
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
        conf.set(DATE_TO_FILTER, props.getProperty(DATE_TO_FILTER));
        List<String> countFields = new LinkedList<String>();
        for (String countField : props.getProperty(COUNT_FIELDS).split(",")) {
            countFields.add(countField.trim().replaceAll("\"", ""));
        }
        conf.setStrings(COUNT_FIELDS,
                        countFields.toArray(new String[countFields.size()]));
        conf.setBoolean(ROUND_RESULTS,
                props.getProperty(ROUND_RESULTS).equalsIgnoreCase("true") ?
                        true : false);
        
        return conf;
    }
}
