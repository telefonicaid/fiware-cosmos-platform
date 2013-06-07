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

package es.tid.cosmos.injection.sftp;

import java.io.File;

import org.apache.commons.cli.ParseException;
import org.apache.commons.configuration.ConfigurationException;
import org.apache.log4j.Logger;
import org.apache.sshd.server.PublickeyAuthenticator;

import es.tid.cosmos.injection.sftp.auth.DaoPublicKeyAuthenticator;
import es.tid.cosmos.injection.sftp.auth.UsersDao;
import es.tid.cosmos.injection.sftp.config.Configuration;
import es.tid.cosmos.injection.sftp.persistence.file.FileBackedUsersDao;
import es.tid.cosmos.injection.sftp.persistence.file.LocalFileSshKeyReader;

/**
 * InjectionServerMain is the main entry point to this application
 *
 * @author logc
 */
public final class InjectionServerMain {

    private static final String DEFAULT_EXTERNAL_CONFIGURATION =
            "file:///etc/cosmos/injection.properties";
    private static final String INTERNAL_CONFIGURATION =
            "/injection_server.prod.properties";
    private static final Logger LOGGER =
            Logger.getLogger(InjectionServerMain.class);

    private InjectionServerMain() {}

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
            config = new Configuration(
                    new File(externalConfiguration).toURI().toURL());
        } catch(Exception ex) {
            config = new Configuration(
                    InjectionServerMain.class.getResource(
                            INTERNAL_CONFIGURATION));
        }

        try {
            InjectionServer server = new InjectionServer(
                    config, setupPublicKeyAuthenticator());
            server.setupSftpServer();
        } catch (Exception ex) {
            LOGGER.error(ex.getMessage(), ex);
            System.exit(1);
        }
    }

    private static PublickeyAuthenticator setupPublicKeyAuthenticator() {
        UsersDao userDao = new FileBackedUsersDao(new LocalFileSshKeyReader());
        return new DaoPublicKeyAuthenticator(userDao);
    }
}
