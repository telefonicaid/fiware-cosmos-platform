package es.tid.ps.kpicalculation.data;

import java.util.Collection;

/**
 * This class implements the factory pattern, to create WebLogs of different
 * types, depending on the use they will have.
 * 
 * @author javierb@tid.es
 */
public abstract class WebLogFactory {
    /**
     * This method creates a weblog instance, and returns it.
     */
    public static WebLog getWebLog(Collection<String> primaryKeys,
            String secondaryKeys, WebLogType wlType) {
        switch (wlType) {
        case WEB_LOG:
            return new WebLog();
        case WEB_LOG_COUNTER:
            return new WebLogCounter(primaryKeys);
        case WEB_LOG_COUNTER_GROUP:
            return new WebLogCounterGroup(primaryKeys, secondaryKeys);
        default:
            return null;
        }
    }
}
