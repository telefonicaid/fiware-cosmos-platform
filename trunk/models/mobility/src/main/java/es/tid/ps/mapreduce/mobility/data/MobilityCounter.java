package es.tid.ps.mapreduce.mobility.data;

/**
 * Enum in where define the counters that we are going to monitor into the
 * cluster. <br>
 * LINE_PARSER_USER_ERROR: the wrong lines into the user file. <br>
 * LINE_PARSER_CDRS_ERROR: the wrong lines into the cdrs file. <br>
 * USER_NOT_IN_PLATFORM: users that appears into the cdrs files but not into the
 * users files.
 * 
 * @author rgc
 */
public enum MobilityCounter {
    LINE_PARSER_USER_ERROR, LINE_PARSER_CDRS_ERROR, USER_NOT_IN_PLATFORM
}
