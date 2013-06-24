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

package es.tid.cosmos.tests.datainjection;

import com.jcraft.jsch.*;

/**
 *
 * @author ximo
 */
public final class CosmosSftp {
    private CosmosSftp() {
    }

    public static Session createSession(Environment env)
            throws JSchException {
        return CosmosSftp.createSession(
                env,
                env.getProperty(EnvironmentSetting.DefaultUser),
                env.getProperty(EnvironmentSetting.DefaultPassword));
    }

    public static Session createSession(Environment env, String username,
                                        String password)
            throws JSchException {
        JSch jsch = new JSch();
        Session session = jsch.getSession(
                username,
                env.getProperty(EnvironmentSetting.SftpUrl),
                Integer.parseInt(env.getProperty(EnvironmentSetting.SftpPort)));

        session.setConfig("StrictHostKeyChecking", "no");
        session.setPassword(password);
        session.connect();
        return session;
    }

    public static ChannelSftp connectToSftp(Session session)
            throws JSchException {
        Channel channel = session.openChannel("sftp");
        channel.connect();
        return (ChannelSftp)channel;
    }
}
