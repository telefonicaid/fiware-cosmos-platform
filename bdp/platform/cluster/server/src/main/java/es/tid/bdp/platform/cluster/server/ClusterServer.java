package es.tid.bdp.platform.cluster.server;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.RunJar;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TServerSocket;

/**
 *
 * @author dmicol
 */
public class ClusterServer implements Cluster.Iface {
    private static final String CONFIG_FILE = "/cluster_server.properties";
    
    private final String notificationEmail;
    private final String hdfsUrl;
    private final String jobtrackerUrl;
    
    private JobRunner jobRunner;
    private Configuration conf;

    public static void main(String args[]) {
        try {
            ClusterServer server = new ClusterServer();
            server.start();
        } catch (Exception ex) {
            System.err.println(ClusterServerUtil.getFullExceptionInformation(ex));
            System.exit(1);
        }
    }

    public ClusterServer() throws IOException {
        Properties props = new Properties();
        props.load(ClusterServer.class.getResource(CONFIG_FILE).openStream());
        this.notificationEmail = props.getProperty("NOTIFICATION_EMAIL");
        this.hdfsUrl = props.getProperty("HDFS_URL");
        this.jobtrackerUrl = props.getProperty("JOBTRACKER_URL");
        
        this.jobRunner = new JobRunner();
        this.conf = new Configuration();
        // TODO: this might not be necessary
        this.conf.set("fs.default.name", this.hdfsUrl);
        this.conf.set("mapred.job.tracker", this.jobtrackerUrl);
        
        ClusterServerUtil.disallowExitCalls();
    }

    private void start() throws Exception {
        TServerSocket serverTransport = new TServerSocket(9888);
        Cluster.Processor processor = new Cluster.Processor(this);
        TThreadPoolServer.Args args = new TThreadPoolServer.Args(serverTransport);
        args.processor(processor);
        TServer server = new TThreadPoolServer(args);
        server.serve();
    }
    
    @Override
    public void copyToHdfs(String src, String dest) throws TransferException {
        try {
            FileSystem fs = FileSystem.get(this.conf);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
            ClusterServerUtil.sendNotificationEmail(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.FILE_COPY_FAILED,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }

    @Override
    public String runJob(String jarPath, String inputPath, String outputPath,
                         String mongoUrl) throws TransferException {
        try {
            int jobId = this.jobRunner.startNewThread(
                    new String[] { jarPath, this.hdfsUrl + "/" + inputPath,
                                   this.hdfsUrl + "/" + outputPath, mongoUrl });
            return String.valueOf(jobId);
        } catch (Exception ex) {
            ClusterServerUtil.sendNotificationEmail(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.RUN_JOB_FAILED,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }

    @Override
    public ClusterJobResult getJobResult(String jobId)
            throws TransferException {
        try {
            return this.jobRunner.getResult(Integer.parseInt(jobId));
        } catch (Exception ex) {
            ClusterServerUtil.sendNotificationEmail(this.notificationEmail, ex);
            throw new TransferException(
                    ClusterErrorCode.INVALID_JOB_ID,
                    ClusterServerUtil.getFullExceptionInformation(ex));
        }
    }
    
    private class JobRunner {
        private static final int MAX_THREADS = 10;
        
        private ExecutorService threadPool = Executors.newFixedThreadPool(
                MAX_THREADS);
        private List<Future<ClusterJobResult>> results =
                new ArrayList<Future<ClusterJobResult>>();

        public synchronized int startNewThread(String[] args) {
            Future<ClusterJobResult> status = this.threadPool.submit(
                    new Job(args));
            this.results.add(status);
            return (this.results.size() - 1);
        }
        
        public ClusterJobResult getResult(int id) throws InterruptedException, 
                                                         ExecutionException {
            ClusterJobResult result;
            Future<ClusterJobResult> resultFuture = this.results.get(id);
            if (resultFuture.isDone()) {
                result = resultFuture.get();
            } else {
                result = new ClusterJobResult();
                result.setStatus(ClusterJobStatus.RUNNING);
            }
            return result;
        }
    }
    
    private class Job implements Callable<ClusterJobResult> {
        private String[] args;
        
        public Job(String[] args) {
            this.args = args.clone();
        }

        @Override
        public ClusterJobResult call() {
            ClusterJobResult result = new ClusterJobResult();
            try {
                RunJar.main(this.args);
                result.setStatus(ClusterJobStatus.SUCCESSFUL);
            } catch (ClusterServerUtil.ExitWithSuccessCodeException ex) {
                result.setStatus(ClusterJobStatus.SUCCESSFUL);
            } catch (ClusterServerUtil.ExitWithFailureCodeException ex) {
                result.setStatus(ClusterJobStatus.FAILED);
                result.setReason(new TransferException(
                        ClusterErrorCode.RUN_JOB_FAILED, "Unknown error"));
            } catch (Throwable ex) {
                result.setStatus(ClusterJobStatus.FAILED);
                result.setReason(new TransferException(
                        ClusterErrorCode.RUN_JOB_FAILED,
                        ClusterServerUtil.getFullExceptionInformation(ex)));
            }
            return result;
        }
    }
}
