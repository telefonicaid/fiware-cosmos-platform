package es.tid.cosmos.mobility;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;

/**
 *
 * @author dmicol
 */
public abstract class Config {
    public static final String MIN_TOTAL_CALLS = "mob.min_total_calls";
    public static final String MAX_TOTAL_CALLS = "mob.max_total_calls";
    public static final String MAX_BTS_AREA = "mob.max_bts_area";
    public static final String MAX_COMMS_BTS = "mob.max_comms_bts";
    public static final String HOME_LABELGROUP_ID = "mob.home_labelgroup_id";
    public static final String MIN_DIST_SECOND_HOME =
            "mob.min_dist_second_home";
    public static final String MIN_PERC_REP_BTS = "mob.min_perc_rep_bts";
    public static final String MIN_NUMBER_CALLS_BTS =
            "mob.min_number_calls_bts";
    public static final String MAX_CDRS = "mob.max_cdrs";
    public static final String PERC_ABSOLUTE_MAX = "mob.perc_absolute_max";
    public static final String MAX_MINUTES_IN_MOVES =
            "mob.max_minutes_in_moves";
    public static final String MIN_MINUTES_IN_MOVES =
            "mob.min_minutes_in_moves";
    public static final String MIN_ITIN_MOVES = "mob.min_itin_moves";
    
    private Config() {
    }
    
    public static Configuration load(InputStream configInput,
            final Configuration configuration) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        
        Configuration conf = new Configuration(configuration);
        conf.setInt(MIN_TOTAL_CALLS, Integer.parseInt(props.getProperty(
                MIN_TOTAL_CALLS)));
        conf.setInt(MAX_TOTAL_CALLS, Integer.parseInt(props.getProperty(
                MAX_TOTAL_CALLS)));
        // Unfortunate since Hadoop's Configuration doesn't have setDouble
        conf.set(MAX_BTS_AREA, props.getProperty(MAX_BTS_AREA));
        conf.setLong(MAX_COMMS_BTS, Long.parseLong(props.getProperty(
                MAX_COMMS_BTS)));
        conf.setInt(HOME_LABELGROUP_ID, Integer.parseInt(props.getProperty(
                HOME_LABELGROUP_ID)));
        // Unfortunate since Hadoop's Configuration doesn't have setDouble
        conf.set(MIN_DIST_SECOND_HOME, props.getProperty(MIN_DIST_SECOND_HOME));
        conf.setInt(MIN_PERC_REP_BTS, Integer.parseInt(props.getProperty(
                MIN_PERC_REP_BTS)));
        conf.setInt(MIN_NUMBER_CALLS_BTS, Integer.parseInt(props.getProperty(
                MIN_NUMBER_CALLS_BTS)));
        conf.setInt(MAX_CDRS, Integer.parseInt(props.getProperty(
                MAX_CDRS)));
        // Unfortunate since Hadoop's Configuration doesn't have setDouble
        conf.set(PERC_ABSOLUTE_MAX, props.getProperty(PERC_ABSOLUTE_MAX));
        conf.setInt(MAX_MINUTES_IN_MOVES, Integer.parseInt(props.getProperty(
                MAX_MINUTES_IN_MOVES)));
        conf.setInt(MIN_MINUTES_IN_MOVES, Integer.parseInt(props.getProperty(
                MIN_MINUTES_IN_MOVES)));
        conf.set(MIN_ITIN_MOVES, props.getProperty(MIN_ITIN_MOVES));
        
        return conf;
    }
}
