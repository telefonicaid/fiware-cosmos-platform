package es.tid.bdp.platform.cluster.server;

import java.io.PrintWriter;
import java.io.StringWriter;
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
        } catch (Exception ex) {
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

    
    private void start() throws Exception {
        TServerSocket serverTransport = new TServerSocket(9888);
        Cluster.Processor processor = new Cluster.Processor(this);
        Args args = new Args(serverTransport);
        args.processor(processor);
        TServer server = new TThreadPoolServer(args);
        server.serve();
    }

    private void sendNotificationEmail(Exception exception) {
        StringWriter writer = new StringWriter();
        exception.printStackTrace(new PrintWriter(writer));
        String errorStack = writer.toString();
        
        String text = "Cosmos failed in production :(\n\n"
                + "The error message was: " + exception.toString() + "\n"
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
            System.err.println(ex.toString());
        }
    }

    @Override
    public void copyToHdfs(String src, String dest) throws TransferException {
        try {
            FileSystem fs = FileSystem.get(this.conf);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
            this.sendNotificationEmail(ex);
            throw new TransferException(ClusterErrorCode.FILE_COPY_FAILED,
                                        ex.toString());
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
        } catch (Exception ex) {
            this.sendNotificationEmail(ex);
            throw new TransferException(ClusterErrorCode.RUN_JOB_FAILED,
                                        ex.toString());
        }
    }

    @Override
    public ClusterJobResult getJobResult(String jobId)
            throws TransferException {
        try {
            return this.jobRunner.getResult(Integer.parseInt(jobId));
        } catch (Exception ex) {
            this.sendNotificationEmail(ex);
            throw new TransferException(ClusterErrorCode.INVALID_JOB_ID,
                                        ex.toString());
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
            } catch (ExitWithSuccessCodeException ex) {
                result.setStatus(ClusterJobStatus.SUCCESSFUL);
            } catch (ExitWithFailureCodeException ex) {
                result.setStatus(ClusterJobStatus.FAILED);
                result.setReason(new TransferException(
                        ClusterErrorCode.RUN_JOB_FAILED, "Unknown error"));
            } catch (Throwable ex) {
                result.setStatus(ClusterJobStatus.FAILED);
                result.setReason(new TransferException(
                        ClusterErrorCode.RUN_JOB_FAILED, ex.toString()));
            }
            return result;
        }
    }
}
