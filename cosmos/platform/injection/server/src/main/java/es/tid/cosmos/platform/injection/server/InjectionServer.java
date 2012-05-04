package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
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

/**
 *
 * @author logc
 */
public class InjectionServer {
    //private static final Logger LOG = Logger.getLogger(InjectionServer.class);
    private static final String CONFIG_FILE = "/injection_server.properties";

//    private final String notificationEmail;
//    private final URI hdfsURI;
    private final int serverSocketPort;

    private Configuration conf;

    public InjectionServer() throws IOException, URISyntaxException {
        Properties props = new Properties();
        props.load(InjectionServer.class.getResource(CONFIG_FILE).openStream());
//        this.notificationEmail = props.getProperty("NOTIFICATION_EMAIL");
//        this.hdfsURI = new URI(props.getProperty("HDFS_URL"));
        this.serverSocketPort = Integer.parseInt(
                props.getProperty("SERVER_SOCKET_PORT"));
//        String jobtrackerUrl = props.getProperty("JOBTRACKER_URL");
//
//        this.conf = new Configuration();
//        // TODO: this might not be necessary
//        this.conf.set("fs.default.name", this.hdfsURI.toString());
//        this.conf.set("mapred.job.tracker", jobtrackerUrl);
    }

    public void setupSftpServer(){
        SshServer sshd = SshServer.setUpDefaultServer();
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
        namedFactoryList.add(new HdfsSftpSubsystem.Factory());
        sshd.setSubsystemFactories(namedFactoryList);

        try {
            sshd.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void start() {
//        LOG.info("Initializing cluster server");
//        TServerSocket serverTransport = new TServerSocket(this.serverSocketPort);
//        Injection.Processor processor = new Injection.Processor(this);
//        TThreadPoolServer.Args args = new TThreadPoolServer.Args(
//                serverTransport);
//        args.processor(processor);
//        TServer server = new TThreadPoolServer(args);
//        
//        InjectionServerUtil.disallowExitCalls();
//        server.serve();
    }

//    @Override
    public void copyToHdfs(String src, String dest) {
//        try {
//            FileSystem fs = FileSystem.get(this.conf);
//            fs.moveFromLocalFile(new Path(src), new Path(dest));
//        } catch (Exception ex) {
//            InjectionServerUtil.logFatalError(this.notificationEmail, ex);
//            throw new TransferException(
//                    InjectionErrorCode.FILE_COPY_FAILED,
//                    InjectionServerUtil.getFullExceptionInformation(ex));
//        }
    }

//    @Override
    public void runJob(String id, String jarPath, String inputPath,
                       String outputPath, String mongoUrl) {
//        try {
//            Job job = new Job(new String[] { jarPath,
//                                             this.absoluteHdfsPath(inputPath),
//                                             this.absoluteHdfsPath(outputPath),
//                                             mongoUrl });
//            this.jobRunner.startNewThread(id, job);
//        } catch (Exception ex) {
//            InjectionServerUtil.logFatalError(this.notificationEmail, ex);
//            throw new TransferException(
//                    InjectionErrorCode.RUN_JOB_FAILED,
//                    InjectionServerUtil.getFullExceptionInformation(ex));
//        }
    }

    protected void absoluteHdfsPath(String relativePath) {
//        return this.hdfsURI.resolve(relativePath).toString();
    }
}
