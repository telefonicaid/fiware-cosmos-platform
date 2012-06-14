package es.tid.cosmos.mobility;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;

import es.tid.cosmos.base.util.Logger;

/**
 *
 * @author dmicol
 */
public abstract class Config {
    public static final String SYS_INPUT_FOLDER = "mob.sys_input_folder";
    public static final String SYS_OUTPUT_TRAINING_FOLDER =
            "mob.sys_output_training_folder";
    public static final String SYS_OUTPUT_COMPLETE_FOLDER =
            "mob.sys_output_complete_folder";
    public static final String SYS_EXEC_MODE = "mob.sys_exec_mode";
    public static final String SYS_EXEC_INCREMENTAL =
            "mob.sys_exec_incremental";
    
    public static final String DATA_SEPARATOR = "mob.data_separator";
    public static final String DATA_DATE_FORMAT = "mob.data_date_format";
    public static final String DATA_START_DATE = "mob.data_start_date";
    public static final String DATA_END_DATE = "mob.data_end_date";
    
    public static final String MIN_TOTAL_CALLS = "mob.client_min_total_calls";
    public static final String MAX_TOTAL_CALLS = "mob.client_max_total_calls";
    public static final String BTS_MAX_BTS_AREA = "mob.bts_max_bts_area";
    public static final String BTS_MAX_COMMS_BTS = "mob.bts_max_comms_bts";
    public static final String POI_HOME_LABELGROUP_ID =
            "mob.poi_home_labelgroup_id";
    public static final String POI_MIN_DIST_SECOND_HOME =
            "mob.poi_min_dist_second_home";
    public static final String POI_MIN_PERC_REP_BTS =
            "mob.poi_min_perc_rep_bts";
    public static final String POI_MIN_NUMBER_CALLS_BTS =
            "mob.poi_min_number_calls_bts";
    public static final String ITIN_PERC_ABSOLUTE_MAX =
            "mob.itin_perc_absolute_max";
    public static final String ITIN_MAX_MINUTES_IN_MOVES =
            "mob.itin_max_minutes_in_moves";
    public static final String ITIN_MIN_MINUTES_IN_MOVES =
            "mob.itin_min_minutes_in_moves";
    public static final String ITIN_MIN_ITIN_MOVES = "mob.itin_min_moves";
    public static final String MTX_MAX_MINUTES_IN_MOVES =
            "mob.mtx_max_minutes_in_moves";
    public static final String MTX_MIN_MINUTES_IN_MOVES =
            "mob.mtx_min_minutes_in_moves";
    public static final String MTX_INCLUDE_INTRA_MOVES =
            "mob.mtx_include_intra_moves";
    
    private Config() {
    }
    
    public static Configuration load(InputStream configInput,
            final Configuration configuration) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        
        Configuration conf = new Configuration(configuration);
        final Field[] fields = Config.class.getDeclaredFields();
        for (Field field : fields) {
            setField(props, conf, field);
        }
        
        return conf;
    }
    
    private static void setField(Properties props, Configuration conf,
                                 Field field) {
        final String name;
        try {
            name = (String) field.get(null);
        } catch (Exception ex) {
            throw new IllegalArgumentException("Could not get property name " +
                                               "for " + field.getName());
        }
        
        final String value = props.getProperty(name);
        if (value == null) {
            throw new IllegalArgumentException("Missing value for property "
                                               + name);
        } else if (value.isEmpty()) {
            Logger.get(Config.class).warn("Value not set for property "
                                          + field.getName());
        }
        
        // The logic of the following code is to try to match the property
        // value to a given data type, from more strict to less. Therefore, 
        // the order is int - long - float - boolean - string.
        try {
            conf.setInt(name, Integer.parseInt(value));
            return;
        } catch (NumberFormatException ex) {
        }
        try {
            conf.setLong(name, Long.parseLong(value));
            return;
        } catch (NumberFormatException ex) {
        }
        try {
            conf.setFloat(name, Float.parseFloat(value));
            return;
        } catch (NumberFormatException ex) {
        }
        
        // For booleans, we don't use Boolean.parseBoolean. This is because it
        // will return false for all strings different than "true", even if they
        // don't represent a real boolean value.
        if ("true".equalsIgnoreCase(value)) {
            conf.setBoolean(name, true);
        } else if ("false".equalsIgnoreCase(value)) {
            conf.setBoolean(name, false);
        } else {
            // If we couldn't match the value to a given data type, we fall back
            // to a string.
            conf.set(name, props.getProperty(name));
        }
    }
}
