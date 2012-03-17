package es.tid.test.hbase.server;

import org.mortbay.jetty.Server;
import org.mortbay.jetty.webapp.WebAppContext;

/**
 *
 * @author sortega
 */
public class ServerMain {
    private static String contextPath = "/";
    private static String resourceBase = "/";
    private static int httpPort = 8888;
    
    public static void main(String[] args) throws Exception {
        Server server = new Server(httpPort);
        WebAppContext webapp = new WebAppContext();
        webapp.setContextPath(contextPath);
        //webapp.setResourceBase(resourceBase);
        webapp.setClassLoader(Thread.currentThread().getContextClassLoader());
        server.setHandler(webapp);
        server.start();
        server.join();
        System.out.println("Started Jetty " + Server.getVersion()
                + ", go to http://localhost:" + httpPort + contextPath);
    }
}
