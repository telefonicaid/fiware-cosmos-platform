package es.tid.cosmos.mobility;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;

/**
 *
 * @author dmicol
 */
public class MobilityConfiguration extends Configuration {
    private static final String SYS_INPUT_FOLDER = "mob.sys_input_folder";
    private static final String SYS_OUTPUT_TRAINING_FOLDER =
            "mob.sys_output_training_folder";
    private static final String SYS_OUTPUT_COMPLETE_FOLDER =
            "mob.sys_output_complete_folder";
    private static final String SYS_EXEC_MODE = "mob.sys_exec_mode";
    private static final String SYS_EXEC_INCREMENTAL =
            "mob.sys_exec_incremental";
    
    private static final String DATA_SEPARATOR = "mob.data_separator";
    private static final String DATA_DATE_FORMAT = "mob.data_date_format";
    private static final String DATA_START_DATE = "mob.data_start_date";
    private static final String DATA_END_DATE = "mob.data_end_date";
    
    private static final String CLIENT_MIN_TOTAL_CALLS =
            "mob.client_min_total_calls";
    private static final String CLIENT_MAX_TOTAL_CALLS =
            "mob.client_max_total_calls";
    private static final String BTS_MAX_BTS_AREA = "mob.bts_max_bts_area";
    private static final String BTS_MIN_COMMS_BTS = "mob.bts_min_comms_bts";
    private static final String POI_HOME_LABELGROUP_ID =
            "mob.poi_home_labelgroup_id";
    private static final String POI_MIN_DIST_SECOND_HOME =
            "mob.poi_min_dist_second_home";
    private static final String POI_MIN_PERC_REP_BTS =
            "mob.poi_min_perc_rep_bts";
    private static final String POI_MIN_NUMBER_CALLS_BTS =
            "mob.poi_min_number_calls_bts";
    private static final String ITIN_PERC_ABSOLUTE_MAX =
            "mob.itin_perc_absolute_max";
    private static final String ITIN_MAX_MINUTES_IN_MOVES =
            "mob.itin_max_minutes_in_moves";
    private static final String ITIN_MIN_MINUTES_IN_MOVES =
            "mob.itin_min_minutes_in_moves";
    private static final String ITIN_MIN_MOVES = "mob.itin_min_moves";
    private static final String MTX_MAX_MINUTES_IN_MOVES =
            "mob.mtx_max_minutes_in_moves";
    private static final String MTX_MIN_MINUTES_IN_MOVES =
            "mob.mtx_min_minutes_in_moves";
    private static final String MTX_INCLUDE_INTRA_MOVES =
            "mob.mtx_include_intra_moves";
    
    public MobilityConfiguration() {
    }
    
    public MobilityConfiguration(Configuration conf) {
        super(conf);
    }
    
    public void load(InputStream configInput) throws IOException {
        Properties props = new Properties();
        props.load(configInput);
        
        this.setSysInputFolder(props.getProperty(SYS_INPUT_FOLDER));
        this.setSysOutputTrainingFolder(props.getProperty(
                SYS_OUTPUT_TRAINING_FOLDER));
        this.setSysOutputCompleteFolder(props.getProperty(
                SYS_OUTPUT_COMPLETE_FOLDER));
        this.setSysExecMode(props.getProperty(SYS_EXEC_MODE));
        this.setSysExecIncremental(props.getProperty(SYS_EXEC_INCREMENTAL));
        
        this.setDataSeparator(props.getProperty(DATA_SEPARATOR));
        this.setDataDateFormat(props.getProperty(DATA_DATE_FORMAT));
        this.setDataStartDate(props.getProperty(DATA_START_DATE));
        this.setDataEndDate(props.getProperty(DATA_END_DATE));
        
        this.setClientMinTotalCalls(props.getProperty(CLIENT_MIN_TOTAL_CALLS));
        this.setClientMaxTotalCalls(props.getProperty(CLIENT_MAX_TOTAL_CALLS));
        this.setBtsMaxBtsArea(props.getProperty(BTS_MAX_BTS_AREA));
        this.setBtsMinCommsBts(props.getProperty(BTS_MIN_COMMS_BTS));
        this.setPoiHomeLabelgroupId(props.getProperty(POI_HOME_LABELGROUP_ID));
        this.setPoiMinDistSecondHome(props.getProperty(
                POI_MIN_DIST_SECOND_HOME));
        this.setPoiMinPercRepBts(props.getProperty(POI_MIN_PERC_REP_BTS));
        this.setPoiMinNumberCallsBts(props.getProperty(
                POI_MIN_NUMBER_CALLS_BTS));
        this.setItinPercAbsoluteMax(props.getProperty(ITIN_PERC_ABSOLUTE_MAX));
        this.setItinMaxMinutesInMoves(props.getProperty(
                ITIN_MAX_MINUTES_IN_MOVES));
        this.setItinMinMinutesInMoves(props.getProperty(
                ITIN_MIN_MINUTES_IN_MOVES));
        this.setItinMinMoves(props.getProperty(ITIN_MIN_MOVES));
        this.setMtxMaxMinutesInMoves(props.getProperty(
                MTX_MAX_MINUTES_IN_MOVES));
        this.setMtxMinMinutesInMoves(props.getProperty(
                MTX_MIN_MINUTES_IN_MOVES));
        this.setMtxIncludeIntraMoves(props.getProperty(
                MTX_INCLUDE_INTRA_MOVES));
    }
    
    public void setSysInputFolder(String value) {
        this.set(SYS_INPUT_FOLDER, value);
    }

    public void setSysOutputTrainingFolder(String value) {
        this.set(SYS_OUTPUT_TRAINING_FOLDER, value);
    }

    public void setSysOutputCompleteFolder(String value) {
        this.set(SYS_OUTPUT_COMPLETE_FOLDER, value);
    }

    public void setSysExecMode(String value) {
        this.set(SYS_EXEC_MODE, value);
    }

    public void setSysExecIncremental(String value) {
        this.setBoolean(SYS_EXEC_INCREMENTAL, Boolean.parseBoolean(value));
    }
    
    public void setDataSeparator(String value) {
        this.set(DATA_SEPARATOR, value);
    }

    public void setDataDateFormat(String value) {
        this.set(DATA_DATE_FORMAT, value);
    }

    public void setDataStartDate(String value) {
        this.set(DATA_START_DATE, value);
    }

    public void setDataEndDate(String value) {
        this.set(DATA_END_DATE, value);
    }

    public void setClientMinTotalCalls(String value) {
        this.setInt(CLIENT_MIN_TOTAL_CALLS, Integer.parseInt(value));
    }

    public void setClientMaxTotalCalls(String value) {
        this.setInt(CLIENT_MAX_TOTAL_CALLS, Integer.parseInt(value));
    }

    public void setBtsMaxBtsArea(String value) {
        this.setFloat(BTS_MAX_BTS_AREA, Float.parseFloat(value));
    }
    
    public void setBtsMinCommsBts(String value) {
        this.setInt(BTS_MIN_COMMS_BTS, Integer.parseInt(value));
    }
    
    public void setPoiHomeLabelgroupId(String value) {
        this.setInt(POI_HOME_LABELGROUP_ID, Integer.parseInt(value));
    }

    public void setPoiMinDistSecondHome(String value) {
        this.setFloat(POI_MIN_DIST_SECOND_HOME, Float.parseFloat(value));
    }

    public void setPoiMinPercRepBts(String value) {
        this.setInt(POI_MIN_PERC_REP_BTS, Integer.parseInt(value));
    }

    public void setPoiMinNumberCallsBts(String value) {
        this.setInt(POI_MIN_NUMBER_CALLS_BTS, Integer.parseInt(value));
    }

    public void setItinPercAbsoluteMax(String value) {
        this.setFloat(ITIN_PERC_ABSOLUTE_MAX, Float.parseFloat(value));
    }

    public void setItinMaxMinutesInMoves(String value) {
        this.setInt(ITIN_MAX_MINUTES_IN_MOVES, Integer.parseInt(value));
    }

    public void setItinMinMinutesInMoves(String value) {
        this.setInt(ITIN_MIN_MINUTES_IN_MOVES, Integer.parseInt(value));
    }

    public void setItinMinMoves(String value) {
        this.setFloat(ITIN_MIN_MOVES, Float.parseFloat(value));
    }

    public void setMtxMaxMinutesInMoves(String value) {
        this.setInt(MTX_MAX_MINUTES_IN_MOVES, Integer.parseInt(value));
    }

    public void setMtxMinMinutesInMoves(String value) {
        this.setInt(MTX_MIN_MINUTES_IN_MOVES, Integer.parseInt(value));
    }

    public void setMtxIncludeIntraMoves(String value) {
        this.setBoolean(MTX_INCLUDE_INTRA_MOVES, Boolean.parseBoolean(value));
    }
    
    public String getSysInputFolder() {
        return this.get(SYS_INPUT_FOLDER);
    }

    public String getSysOutputTrainingFolder() {
        return this.get(SYS_OUTPUT_TRAINING_FOLDER);
    }

    public String getSysOutputCompleteFolder() {
        return this.get(SYS_OUTPUT_COMPLETE_FOLDER);
    }

    public String getSysExecMode() {
        return this.get(SYS_EXEC_MODE);
    }

    public boolean getSysExecIncremental() {
        return this.getBoolean(SYS_EXEC_INCREMENTAL, false);
    }
    
    public String getDataSeparator() {
        return this.get(DATA_SEPARATOR);
    }

    public String getDataDateFormat() {
        return this.get(DATA_DATE_FORMAT);
    }

    public String getDataStartDate() {
        return this.get(DATA_START_DATE);
    }

    public String getDataEndDate() {
        return this.get(DATA_END_DATE);
    }

    public int getClientMinTotalCalls() {
        return this.getInt(CLIENT_MIN_TOTAL_CALLS, Integer.MIN_VALUE);
    }

    public int getClientMaxTotalCalls() {
        return this.getInt(CLIENT_MAX_TOTAL_CALLS, Integer.MAX_VALUE);
    }

    public double getBtsMaxBtsArea() {
        return this.getFloat(BTS_MAX_BTS_AREA, Float.MAX_VALUE);
    }
    
    public int getBtsMinCommsBts() {
        return this.getInt(BTS_MIN_COMMS_BTS, Integer.MIN_VALUE);
    }
    
    public int getPoiHomeLabelgroupId() {
        return this.getInt(POI_HOME_LABELGROUP_ID, Integer.MIN_VALUE);
    }

    public double getPoiMinDistSecondHome() {
        return this.getFloat(POI_MIN_DIST_SECOND_HOME, Float.MIN_VALUE);
    }

    public int getPoiMinPercRepBts() {
        return this.getInt(POI_MIN_PERC_REP_BTS, Integer.MIN_VALUE);
    }

    public int getPoiMinNumberCallsBts() {
        return this.getInt(POI_MIN_NUMBER_CALLS_BTS, Integer.MIN_VALUE);
    }

    public double getItinPercAbsoluteMax() {
        return this.getFloat(ITIN_PERC_ABSOLUTE_MAX, Float.MAX_VALUE);
    }

    public int getItinMaxMinutesInMoves() {
        return this.getInt(ITIN_MAX_MINUTES_IN_MOVES, Integer.MAX_VALUE);
    }

    public int getItinMinMinutesInMoves() {
        return this.getInt(ITIN_MIN_MINUTES_IN_MOVES, Integer.MIN_VALUE);
    }

    public double getItinMinMoves() {
        return this.getFloat(ITIN_MIN_MOVES, Float.MAX_VALUE);
    }

    public int getMtxMaxMinutesInMoves() {
        return this.getInt(MTX_MAX_MINUTES_IN_MOVES, Integer.MAX_VALUE);
    }

    public int getMtxMinMinutesInMoves() {
        return this.getInt(MTX_MIN_MINUTES_IN_MOVES, Integer.MAX_VALUE);
    }

    public boolean getMtxIncludeIntraMoves() {
        return this.getBoolean(MTX_INCLUDE_INTRA_MOVES, false);
    }
}
