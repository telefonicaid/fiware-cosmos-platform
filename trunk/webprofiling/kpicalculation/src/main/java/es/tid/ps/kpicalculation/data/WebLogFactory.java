package es.tid.ps.kpicalculation.data;

import java.util.Collection;

/**
 * This class implements the factory pattern, to create WebLogs of different
 * types, depending on the use they will have.
 * 
 * @author javierb@tid.es
 */
public class WebLogFactory {

    public static WebLog instance;

    public static Collection<String> primaryKeys;
    public static String secondaryKeys;

    public static WebLogType type = null;

    /**
     * This method returns a weblog.
     */
    public static WebLog getPageView() {

        if (type == WebLogType.WEB_LOG) {
            return new WebLog();
        } else if (type == WebLogType.WEB_LOG_COUNTER) {

            return new WebLogCounter(primaryKeys);
        } else if (type == WebLogType.WEB_LOG_COUNTER_GROUP) {
            return new WebLogCounterGroup(primaryKeys, secondaryKeys);
        }

        return null;
    }

    /**
     * This method sets the values needed in order to provide a type of weblog
     * when it is requested to the factory.
     * 
     * @param mainKey
     *            filtering keys to be used for calculating the kpi
     * @param secKeys
     *            secondary key to be used for grouping when calculating the kpi
     * @param wlType
     *            type of weblog to be returned.
     */
    public static void setKeys(Collection<String> mainKeys, String secKeys,
            WebLogType wlType) {
        primaryKeys = mainKeys;
        secondaryKeys = secKeys;
        type = wlType;

    }
}
