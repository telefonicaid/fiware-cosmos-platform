package es.tid.mapreduce.mobility2.data;

/**
 * Enum to define the counters that we are going to monitor into the
 * cluster. <br>
 * USER_CRDS_EXCEEDED: user with too many CDRS lines (by default more that 5000). <br>
 * LINE_PARSER_CDRS_ERROR: the wrong lines into the cdrs file. <br>
 * 
 * @author rgc
 */
public enum Mobility2Counter {
    USER_CRDS_EXCEED, LINE_PARSER_CDRS_ERROR
}
