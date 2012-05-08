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
import org.apache.sshd.server.auth.UserAuthNone;
import org.apache.sshd.server.command.ScpCommandFactory;
import org.apache.sshd.server.keyprovider.SimpleGeneratorHostKeyProvider;
import org.apache.sshd.server.sftp.SftpSubsystem;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author logc
 */
public class InjectionServer {
    private static Logger LOG;
    private static final String CONFIG_FILE = "/injection_server.properties";
//    private final String notificationEmail;
    private final URI hdfsURI;
    private final int serverSocketPort;
    private Configuration conf;
    private HadoopFileSystemFactory hadoopFileSystemFactory;

    public InjectionServer() throws IOException, URISyntaxException {
        LOG = LoggerFactory.getLogger(InjectionServer.class);
        Properties props = new Properties();
        props.load(InjectionServer.class.getResource(CONFIG_FILE).openStream());
//        this.notificationEmail = props.getProperty("NOTIFICATION_EMAIL");
        this.hdfsURI = new URI(props.getProperty("HDFS_URL"));
        this.serverSocketPort = Integer.parseInt(
                props.getProperty("SERVER_SOCKET_PORT"));
        String jobtrackerUrl = props.getProperty("JOBTRACKER_URL");
        this.conf = new Configuration();
        this.conf.set("fs.default.name", this.hdfsURI.toString());
        this.conf.set("mapred.job.tracker", jobtrackerUrl);
        // TODO: pass conf to hadoopFileSystemFactory
        this.hadoopFileSystemFactory = new HadoopFileSystemFactory();
    }

    public void setupSftpServer(){
        SshServer sshd = SshServer.setUpDefaultServer();
        sshd.setFileSystemFactory(hadoopFileSystemFactory);
        sshd.setPort(this.serverSocketPort);
        sshd.setKeyPairProvider(
                new SimpleGeneratorHostKeyProvider("hostkey.ser"));

        List<NamedFactory<UserAuth>> userAuthFactories =
                new ArrayList<NamedFactory<UserAuth>>();
        userAuthFactories.add(new UserAuthNone.Factory());
        sshd.setUserAuthFactories(userAuthFactories);

        sshd.setCommandFactory(new ScpCommandFactory());

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
