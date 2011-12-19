package es.tid.ps.kpicalculation.data;

/**
 * Enum defining the different WebLog's types. <br>
 * WEB_LOG: Single type used for the filtering and normalization phase
 * WEB_LOG_COUNTER: Type used for calculating basic aggregated kpis
 * WEB_LOG_COUNTER_GROUP: Type used for calculating aggregated kpis by unique
 * field
 * 
 * @author javierb
 */
public enum WebLogType {
    WEB_LOG, 
    WEB_LOG_COUNTER, 
    WEB_LOG_COUNTER_GROUP
}
