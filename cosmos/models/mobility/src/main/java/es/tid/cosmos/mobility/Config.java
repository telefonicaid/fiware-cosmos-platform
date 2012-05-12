package es.tid.cosmos.mobility;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

/**
 *
 * @author dmicol
 */
public abstract class Config {
    public static int minTotalCalls;
    public static int maxTotalCalls;
    public static double maxBtsArea;
    public static long maxCommsBts;
    public static int homeLabelgroupId;
    public static int workLabelgroupId;
    public static double minDistSecondHome;
    public static int minPercRepBts;
    public static int minNumberCallsBts;
    public static int maxCdrs;
    
    private Config() {
    }
    
    public static void load(InputStream configInput) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        minTotalCalls = Integer.parseInt(props.getProperty(
                "MIN_TOTAL_CALLS"));
        maxTotalCalls = Integer.parseInt(props.getProperty(
                "MAX_TOTAL_CALLS"));
        maxBtsArea = Double.parseDouble(props.getProperty(
                "MAX_BTS_AREA"));
        maxCommsBts = Long.parseLong(props.getProperty(
                "MAX_COMMS_BTS"));
        homeLabelgroupId = Integer.parseInt(props.getProperty(
                "HOME_LABELGROUP_ID"));
        workLabelgroupId = Integer.parseInt(props.getProperty(
                "WORK_LABELGROUP_ID"));
        minDistSecondHome = Double.parseDouble(props.getProperty(
                "MIN_DIST_SECOND_HOME"));
        minPercRepBts = Integer.parseInt(props.getProperty(
                "MIN_PERC_REP_BTS"));
        minNumberCallsBts = Integer.parseInt(props.getProperty(
                "MIN_NUMBER_CALLS_BTS"));
        maxCdrs = Integer.parseInt(props.getProperty(
                "MAX_CDRS"));
    }
}
