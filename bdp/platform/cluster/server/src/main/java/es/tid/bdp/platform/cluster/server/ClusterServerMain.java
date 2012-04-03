package es.tid.bdp.platform.cluster.server;

import org.apache.log4j.Logger;

/**
 *
 * @author dmicol
 */
public class ClusterServerMain {
    private static final Logger LOG = Logger.getLogger(ClusterServer.class);
    
    public static void main(String[] args) {
        try {
            ClusterServer server = new ClusterServer();
            server.start();
        } catch (Exception ex) {
            LOG.fatal(ex.getMessage());
            System.exit(1);
        }
    }
    
    private ClusterServerMain() {
    }
}
