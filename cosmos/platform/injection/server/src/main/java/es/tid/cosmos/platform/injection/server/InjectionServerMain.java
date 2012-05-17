package es.tid.cosmos.platform.injection.server;

import es.tid.cosmos.base.util.ArgumentParser;

/**
 * InjectionServerMain is the main entry point to this application
 *
 * @author logc
 * @since  CTP 2
 */
public final class InjectionServerMain {
    private String appName = "Injection Server";

    private InjectionServerMain() {
    }

    public static void main(String[] args) {
        try {
            ArgumentParser argumentParser = new ArgumentParser();
            argumentParser.parse(args);

        } catch (IllegalArgumentException e) {
            System.out.println();
        }
        try {
            InjectionServer server = new InjectionServer();
            server.setupSftpServer();
        } catch (Exception ex) {
            System.exit(1);
        }
    }
}
