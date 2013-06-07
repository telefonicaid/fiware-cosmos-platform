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

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.log4j.Logger;
import org.apache.sshd.SshServer;
import org.apache.sshd.common.NamedFactory;
import org.apache.sshd.server.Command;
import org.apache.sshd.server.PublickeyAuthenticator;
import org.apache.sshd.server.UserAuth;
import org.apache.sshd.server.auth.UserAuthPassword;
import org.apache.sshd.server.auth.UserAuthPublicKey;
import org.apache.sshd.server.command.ScpCommandFactory;
import org.apache.sshd.server.keyprovider.SimpleGeneratorHostKeyProvider;
import org.apache.sshd.server.sftp.SftpSubsystem;

import es.tid.cosmos.injection.sftp.config.Configuration;
import es.tid.cosmos.injection.sftp.hadoopfs.HadoopFileSystemFactory;

/**
 * InjectionServer connects an SFTP client to an HDFS filesystem
 *
 * @author logc
 */
public class InjectionServer {

    private static final Logger LOGGER =
            Logger.getLogger(InjectionServer.class);

    private HadoopFileSystemFactory hadoopFileSystemFactory;
    private final Configuration configuration;
    private final PublickeyAuthenticator publicKeyAuthenticator;

    /**
     * Constructs this instance from the configured values
     *
     * @throws IOException
     * @throws URISyntaxException
     */
    public InjectionServer(
            Configuration serverConfig,
            PublickeyAuthenticator publicKeyAuthenticator)
            throws IOException, URISyntaxException, ConfigurationException {
        this.configuration = serverConfig;
        org.apache.hadoop.conf.Configuration hadoopConfig =
                new org.apache.hadoop.conf.Configuration();
        hadoopConfig.set("fs.default.name",
                serverConfig.getHdfsUrl().toString());
        hadoopConfig.set("mapred.job.tracker", serverConfig.getJobTrackerUrl());
        this.hadoopFileSystemFactory = new HadoopFileSystemFactory(hadoopConfig);
        this.publicKeyAuthenticator = publicKeyAuthenticator;
    }

    /**
     * Sets up and start an SFTP sftp
     */
    public void setupSftpServer() {
        SshServer sshd = SshServer.setUpDefaultServer();
        // General settings
        sshd.setFileSystemFactory(this.hadoopFileSystemFactory);
        sshd.setPort(this.configuration.getPort());
        sshd.setKeyPairProvider(
                new SimpleGeneratorHostKeyProvider("hostkey.ser"));
        // User authentication settings
        sshd.setPublickeyAuthenticator(this.publicKeyAuthenticator);
        List<NamedFactory<UserAuth>> userAuthFactories =
                new ArrayList<NamedFactory<UserAuth>>();
        userAuthFactories.add(new UserAuthPassword.Factory());
        userAuthFactories.add(new UserAuthPublicKey.Factory());
        sshd.setUserAuthFactories(userAuthFactories);
        // Command settings
        sshd.setCommandFactory(new ScpCommandFactory());
        // Subsystem settings
        List<NamedFactory<Command>> namedFactoryList =
                new ArrayList<NamedFactory<Command>>();
        namedFactoryList.add(new SftpSubsystem.Factory());
        sshd.setSubsystemFactories(namedFactoryList);

        try {
            sshd.start();
        } catch (Exception e) {
            LOGGER.error(e.getLocalizedMessage());
        }
    }
}
