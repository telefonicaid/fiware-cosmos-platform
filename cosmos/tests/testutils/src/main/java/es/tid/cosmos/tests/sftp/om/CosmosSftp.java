package es.tid.cosmos.tests.sftp.om;

import com.jcraft.jsch.*;

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.environment.EnvironmentSetting;

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
