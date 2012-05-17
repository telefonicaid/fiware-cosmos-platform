package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;
import org.apache.sshd.SshServer;
import org.apache.sshd.common.NamedFactory;
import org.apache.sshd.server.Command;
import org.apache.sshd.server.UserAuth;
import org.apache.sshd.server.auth.UserAuthPassword;
import org.apache.sshd.server.command.ScpCommandFactory;
import org.apache.sshd.server.keyprovider.SimpleGeneratorHostKeyProvider;
import org.apache.sshd.server.sftp.SftpSubsystem;

import es.tid.cosmos.base.util.Logger;

/**
 * InjectionServer connects an SFTP client to an HDFS filesystem
 *
 * @author logc
 * @since  CTP 2
 */
public class InjectionServer {
    private static final String CONFIG_FILE = "/injection_server.dev.properties";

    private HadoopFileSystemFactory hadoopFileSystemFactory;
    private final String frontendDbUrl;
    private final int serverSocketPort;
    private final String dbName;
    private final String dbUser;
    private final String dbPassword;
    private final org.apache.log4j.Logger LOG =
            Logger.get(InjectionServer.class);

    /**
     * Constructs this instance from the configured values
     *
     * @throws IOException
     * @throws URISyntaxException
     */
    public InjectionServer() throws IOException, URISyntaxException {
        Properties props = new Properties();
        props.load(InjectionServer.class.getResource(CONFIG_FILE).openStream());
        URI hdfsURI = new URI(props.getProperty("HDFS_URL"));
        this.serverSocketPort = Integer.parseInt(
                props.getProperty("SERVER_SOCKET_PORT"));
        String jobtrackerUrl = props.getProperty("JOBTRACKER_URL");
        this.frontendDbUrl = props.getProperty("FRONTEND_DB");
        this.dbName = props.getProperty("DB_NAME");
        this.dbUser = props.getProperty("DB_USER");
        this.dbPassword = props.getProperty("DB_PASS");
        Configuration conf = new Configuration();
        conf.set("fs.default.name", hdfsURI.toString());
        conf.set("mapred.job.tracker", jobtrackerUrl);
        this.hadoopFileSystemFactory = new HadoopFileSystemFactory(conf);
    }

    /**
     * Sets up and start an SFTP server
     */
    public void setupSftpServer(){
        SshServer sshd = SshServer.setUpDefaultServer();
        // General settings
        sshd.setFileSystemFactory(hadoopFileSystemFactory);
        sshd.setPort(this.serverSocketPort);
        sshd.setKeyPairProvider(
                new SimpleGeneratorHostKeyProvider("hostkey.ser"));
        // User authentication settings
        FrontendPassword passwordAuthenticator = new FrontendPassword();
        passwordAuthenticator.setFrontendCredentials(this.frontendDbUrl,
                this.dbName, this.dbUser, this.dbPassword);
        sshd.setPasswordAuthenticator(passwordAuthenticator);
        List<NamedFactory<UserAuth>> userAuthFactories =
                new ArrayList<NamedFactory<UserAuth>>();
        userAuthFactories.add(new UserAuthPassword.Factory());
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
            LOG.error(e.getLocalizedMessage());
        }
    }
}
