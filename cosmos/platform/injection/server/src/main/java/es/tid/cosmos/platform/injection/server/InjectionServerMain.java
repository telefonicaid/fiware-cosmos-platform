package es.tid.cosmos.platform.injection.server;

/**
 * InjectionServerMain is the main entry point to this application
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since  CTP 2
 */
public final class InjectionServerMain {
    private InjectionServerMain() {
    }

    public static void main(String[] args) {
        try {
            InjectionServer server = new InjectionServer();
            server.setupSftpServer();
        } catch (Exception ex) {
            System.exit(1);
        }
    }
}
