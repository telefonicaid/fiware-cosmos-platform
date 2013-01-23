/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.platform.injection.server;

import java.io.File;

import org.apache.commons.cli.*;
import org.apache.commons.configuration.ConfigurationException;

import es.tid.cosmos.base.util.Logger;

/**
 * InjectionServerMain is the main entry point to this application
 *
 * @author logc
 * @since  CTP 2
 */
public final class InjectionServerMain {
    private static final String DEFAULT_EXTERNAL_CONFIGURATION =
            "file:///etc/cosmos/injection.properties";
    private static final String INTERNAL_CONFIGURATION =
            "/injection_server.prod.properties";
    private static final org.apache.log4j.Logger LOG =
            Logger.get(InjectionServerMain.class);

    private InjectionServerMain() {
    }

    public static void main(String[] args) throws ConfigurationException {
        ServerCommandLine commandLine = new ServerCommandLine();
        try {
            commandLine.parse(args);
        } catch (ParseException e) {
            commandLine.printUsage();
            System.exit(1);
        }

        String externalConfiguration = DEFAULT_EXTERNAL_CONFIGURATION;
        if (commandLine.hasConfigFile()) {
            externalConfiguration = commandLine.getConfigFile();
        }

        Configuration config;
        try {
            config = new Configuration(new File(externalConfiguration)
                                               .toURI().toURL());
        } catch(Exception ex) {
            config = new Configuration(InjectionServerMain.class
                                         .getResource(INTERNAL_CONFIGURATION));
        }

        try {
            InjectionServer server = new InjectionServer(config);
            server.setupSftpServer();
        } catch (Exception ex) {
            LOG.error(ex.getMessage(), ex);
            System.exit(1);
        }
    }


}
