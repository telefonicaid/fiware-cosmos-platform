package es.tid.bdp.platform.cluster.server;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;

import javax.mail.Message;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.RunJar;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.server.TThreadPoolServer.Args;
import org.apache.thrift.transport.TServerSocket;

/**
 *
 * @author dmicol
 */
public class ClusterServer implements Cluster.Iface {
    private static class ExitWithSuccessCodeException
            extends SecurityException { }
    private static class ExitWithFailureCodeException
            extends SecurityException { }
    
    private static final String NOTIFICATION_EMAIL = "cosmos-prod@tid.es";
    
    // TODO: put this in a configuration file
    private static final String HDFS_URL = "hdfs://pshdp01:8011";
    private static final String JOBTRACKER_URL = "pshdp01:8012";
    
    private JobRunner jobRunner;
    private Configuration conf;

    public static void main(String args[]) {
        try {
            ClusterServer server = new ClusterServer();
            server.start();
        } catch (Throwable ex) {
            System.exit(1);
        }
    }

    public ClusterServer() {
        this.disallowExitCalls();
        
        this.jobRunner = new JobRunner();
        this.conf = new Configuration();
        // TODO: this might not be necessary
        this.conf.set("fs.default.name", HDFS_URL);
        this.conf.set("mapred.job.tracker", JOBTRACKER_URL);
    }
    
    private void disallowExitCalls() {
        final SecurityManager securityManager = new SecurityManager() {
            @Override
            public void checkPermission(java.security.Permission permission) {
                if (permission.getName().contains("exitVM.0")) {
                    throw new ExitWithSuccessCodeException();
                } else if (permission.getName().contains("exitVM")) {
                    throw new ExitWithFailureCodeException();
                }
            }
        };
        System.setSecurityManager(securityManager);
    }

    
    private void start() throws Throwable {
        TServerSocket serverTransport = new TServerSocket(9888);
        Cluster.Processor processor = new Cluster.Processor(this);
        Args args = new Args(serverTransport);
        args.processor(processor);
        TServer server = new TThreadPoolServer(args);
        server.serve();
    }

    private void sendNotificationEmail(String errorText, String errorStack) {
        String text = "Cosmos failed in production :(\n\n"
                + "The error message was: " + errorText + "\n"
                + "and the call stack:" + errorStack + "\n\n"
                + "Please fix me!\n";
        
        Properties props = new Properties();
        props.put("mail.smtp.starttls.enable", "false");
        props.put("mail.smtp.host", "mailhost.hi.inet");
        props.put("mail.smtp.port", "25");
        Session session = Session.getInstance(props, null);
        try {
            Message msg = new MimeMessage(session);
            msg.setFrom(new InternetAddress(NOTIFICATION_EMAIL));
            msg.addRecipient(Message.RecipientType.TO,
                             new InternetAddress(NOTIFICATION_EMAIL));
            msg.setSubject("Cosmos Failure");
            msg.setText(text);
            Transport.send(msg);
        } catch (Exception ex) {
            System.err.println(ex.getMessage());
        }
    }

    @Override
    public void copyToHdfs(String src, String dest) throws TransferException {
        try {
            FileSystem fs = FileSystem.get(this.conf);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TransferException(ClusterErrorCode.FILE_COPY_FAILED,
                                        ex.getMessage());
        }
    }

    @Override
    public String runJob(String jarPath, String inputPath, String outputPath,
                         String mongoUrl) throws TransferException {
        try {
            int jobId = this.jobRunner.startNewThread(
                    new String[] { jarPath, HDFS_URL + "/" + inputPath,
                                   HDFS_URL + "/" + outputPath, mongoUrl });
            return String.valueOf(jobId);
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TransferException(ClusterErrorCode.RUN_JOB_FAILED,
                                        ex.getMessage());
        }
    }

    @Override
    public ClusterJobStatus getJobStatus(String jobId)
            throws TransferException {
        try {
            return this.jobRunner.getStatus(Integer.parseInt(jobId));
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TransferException(ClusterErrorCode.INVALID_JOB_ID,
                                        ex.getMessage());
        }
    }
    
    private class JobRunner {
        private static final int MAX_THREADS = 10;
        
        private ExecutorService threadPool = Executors.newFixedThreadPool(
                MAX_THREADS);
        private List<Future<ClusterJobStatus>> jobs =
                new ArrayList<Future<ClusterJobStatus>>();

        public synchronized int startNewThread(String[] args) {
            Future<ClusterJobStatus> status = this.threadPool.submit(
                    new Job(args));
            this.jobs.add(status);
            return (this.jobs.size() - 1);
        }
        
        public ClusterJobStatus getStatus(int id) throws InterruptedException, 
                                                         ExecutionException {
            Future<ClusterJobStatus> jobStatus = this.jobs.get(id);
            if (jobStatus.isDone()) {
                return jobStatus.get();
            } else {
                return ClusterJobStatus.RUNNING;
            }
        }
    }
    
    private class Job implements Callable<ClusterJobStatus> {
        private String[] args;
        
        public Job(String[] args) {
            this.args = args.clone();
        }

        @Override
        public ClusterJobStatus call() throws Exception {
            try {
                RunJar.main(this.args);
                return ClusterJobStatus.SUCCESSFUL;
            } catch (ExitWithSuccessCodeException ex) {
                return ClusterJobStatus.SUCCESSFUL;
            } catch (ExitWithFailureCodeException ex) {
                return ClusterJobStatus.FAILED;
            } catch (Throwable ex) {
                return ClusterJobStatus.FAILED;
            }
        }
    }
}
