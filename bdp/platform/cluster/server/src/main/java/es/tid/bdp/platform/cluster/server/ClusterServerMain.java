package es.tid.bdp.platform.cluster.server;

/**
 *
 * @author dmicol
 */
public class ClusterServerMain {
    public static void main(String[] args) {
        try {
            ClusterServer server = new ClusterServer();
            server.start();
        } catch (Exception ex) {
            System.exit(1);
        }
    }
}
