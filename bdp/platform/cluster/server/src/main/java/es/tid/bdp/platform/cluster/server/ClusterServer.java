package es.tid.bdp.platform.cluster.server;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.log4j.Logger;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TTransportException;

/**
 *
 * @author dmicol
 */
public class ClusterServer implements Cluster.Iface {
    private static final Logger LOG = Logger.getLogger(ClusterServer.class);
    private static final String CONFIG_FILE = "/cluster_server.properties";

    private final String notificationEmail;
    private final URI hdfsURI;
    private final int serverSocketPort;

    private JobRunner jobRunner;
    private Configuration conf;

    public ClusterServer() throws IOException, URISyntaxException {
        Properties props = new Properties();
        props.load(ClusterServer.class.getResource(CONFIG_FILE).openStream());
        this.notificationEmail = props.getProperty("NOTIFICATION_EMAIL");
        this.hdfsURI = new URI(props.getProperty("HDFS_URL"));
        this.serverSocketPort = Integer.parseInt(
                props.getProperty("SERVER_SOCKET_PORT"));
        String jobtrackerUrl = props.getProperty("JOBTRACKER_URL");

        this.jobRunner = new JobRunner();
        this.conf = new Configuration();
        // TODO: this might not be necessary
        this.conf.set("fs.default.name", this.hdfsURI.toString());
        this.conf.set("mapred.job.tracker", jobtrackerUrl);
    }

    public void start() throws TTransportException {
        LOG.info("Initializing cluster server");
        TServerSocket serverTransport = new TServerSocket(this.serverSocketPort);
        Cluster.Processor processor = new Cluster.Processor(this);
        TThreadPoolServer.Args args = new TThreadPoolServer.Args(
                serverTransport);
        args.processor(processor);
        TServer server = new TThreadPoolServer(args);
        
        ClusterServerUtil.disallowExitCalls();
        server.serve();
    }

    @Override
    public void copyToHdfs(String src, String dest) throws TransferException {
        try {
            FileSystem fs = FileSystem.get(this.conf);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
            ClusterServerUtil.logFatalError(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.FILE_COPY_FAILED,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }

    @Override
    public void runJob(String id, String jarPath, String inputPath,
                       String outputPath, String mongoUrl)
            throws TransferException {
        try {
            Job job = new Job(new String[] { jarPath,
                                             this.absoluteHdfsPath(inputPath),
                                             this.absoluteHdfsPath(outputPath),
                                             mongoUrl });
            this.jobRunner.startNewThread(id, job);
        } catch (Exception ex) {
            ClusterServerUtil.logFatalError(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.RUN_JOB_FAILED,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }

    protected String absoluteHdfsPath(String relativePath) {
        return this.hdfsURI.resolve(relativePath).toString();
    }

    @Override
    public ClusterJobResult getJobResult(String jobId)
            throws TransferException {
        try {
            return this.jobRunner.getResult(jobId);
        } catch (Exception ex) {
            ClusterServerUtil.logFatalError(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.INVALID_JOB_ID,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }
}
