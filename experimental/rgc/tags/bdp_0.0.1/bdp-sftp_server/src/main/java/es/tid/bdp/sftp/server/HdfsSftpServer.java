package es.tid.bdp.sftp.server;

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

import java.net.InetSocketAddress;
import java.security.PublicKey;
import java.util.Arrays;
import java.util.EnumSet;

import org.apache.sshd.SshServer;
import org.apache.sshd.common.NamedFactory;
import org.apache.sshd.common.util.OsUtils;
import org.apache.sshd.common.util.SecurityUtils;
import org.apache.sshd.server.Command;
import org.apache.sshd.server.ForwardingFilter;
import org.apache.sshd.server.PasswordAuthenticator;
import org.apache.sshd.server.PublickeyAuthenticator;
import org.apache.sshd.server.keyprovider.PEMGeneratorHostKeyProvider;
import org.apache.sshd.server.keyprovider.SimpleGeneratorHostKeyProvider;
import org.apache.sshd.server.session.ServerSession;
import org.apache.sshd.server.sftp.SftpSubsystem;
import org.apache.sshd.server.shell.ProcessShellFactory;

import es.tid.bdp.sftp.server.filesystem.hadoop.HdfsFileSystemFactory;
import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * 
 * @author rgc
 * 
 */
public class HdfsSftpServer {

    private final static String PROPERTY_PATH = "properties.configuration";
    private final static String PATH_CONFIG_DEFAULT = "sftp-server.properties";
    private static final String SFTP_PORT = "sftp.port";
    private final static int DEFAULT_PORT = 8000;

    public static void main(String[] args) throws Exception {

        String configFile = System.getProperty(PROPERTY_PATH,
                PATH_CONFIG_DEFAULT);

        PropertiesPlaceHolder properties = PropertiesPlaceHolder
                .createInstance(configFile);

        int port = properties.getPropertyInt(SFTP_PORT, DEFAULT_PORT);

        SshServer sshd = SshServer.setUpDefaultServer();
        sshd.setPort(port);
        if (SecurityUtils.isBouncyCastleRegistered()) {
            sshd.setKeyPairProvider(new PEMGeneratorHostKeyProvider("key.pem"));
        } else {
            sshd.setKeyPairProvider(new SimpleGeneratorHostKeyProvider(
                    "key.ser"));
        }
        if (OsUtils.isUNIX()) {
            sshd.setShellFactory(new ProcessShellFactory(new String[] {
                    "/bin/sh", "-i", "-l" }, EnumSet
                    .of(ProcessShellFactory.TtyOptions.ONlCr)));
        } else {
            sshd.setShellFactory(new ProcessShellFactory(
                    new String[] { "cmd.exe " }, EnumSet.of(
                            ProcessShellFactory.TtyOptions.Echo,
                            ProcessShellFactory.TtyOptions.ICrNl,
                            ProcessShellFactory.TtyOptions.ONlCr)));
        }
        sshd.setPasswordAuthenticator(new PasswordAuthenticator() {
            public boolean authenticate(String username, String password,
                    ServerSession session) {
                return username != null && username.equals(password);
            }
        });
        sshd.setPublickeyAuthenticator(new PublickeyAuthenticator() {
            public boolean authenticate(String username, PublicKey key,
                    ServerSession session) {
                // File f = new File("/Users/" + username +
                // "/.ssh/authorized_keys");
                return true;
            }
        });
        sshd.setForwardingFilter(new ForwardingFilter() {
            public boolean canForwardAgent(ServerSession session) {
                return true;
            }

            public boolean canForwardX11(ServerSession session) {
                return true;
            }

            public boolean canListen(InetSocketAddress address,
                    ServerSession session) {
                return true;
            }

            public boolean canConnect(InetSocketAddress address,
                    ServerSession session) {
                return true;
            }
        });

        
        
        sshd.setFileSystemFactory(new HdfsFileSystemFactory());
        sshd.setSubsystemFactories(Arrays
                .<NamedFactory<Command>> asList(new SftpSubsystem.Factory()));
        sshd.start();
    }

}
