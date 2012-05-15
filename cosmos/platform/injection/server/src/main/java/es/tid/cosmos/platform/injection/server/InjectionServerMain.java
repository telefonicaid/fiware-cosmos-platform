package es.tid.cosmos.platform.injection.server;

/**
 *
 * @author logc
 */
public final class InjectionServerMain {

    public static void main(String[] args) {
        try {
            InjectionServer server = new InjectionServer();
            server.setupSftpServer();
        } catch (Exception ex) {
            System.exit(1);
        }
    }
    
    private InjectionServerMain() {
    }
}
