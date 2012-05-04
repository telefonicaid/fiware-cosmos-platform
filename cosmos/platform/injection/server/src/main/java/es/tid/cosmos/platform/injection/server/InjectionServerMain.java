package es.tid.cosmos.platform.injection.server;

/**
 *
 * @author logc
 */
public final class InjectionServerMain {
    //private static final Logger LOG = Logger.getLogger(InjectionServer.class);

    public static void main(String[] args) {
        try {
            InjectionServer server = new InjectionServer();
            server.setupSftpServer();
        } catch (Exception ex) {
            //LOG.fatal(ex.getMessage());
            System.exit(1);
        }
    }
    
    private InjectionServerMain() {
    }
}
