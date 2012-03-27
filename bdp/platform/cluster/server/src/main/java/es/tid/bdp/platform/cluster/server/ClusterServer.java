package es.tid.bdp.platform.cluster.server;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

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
    
    // TODO: put this in a configuration file
    private static final String HDFS_URL = "hdfs://pshdp01:8011";
    private static final String JOBTRACKER_URL = "pshdp01:8012";
    
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
            msg.setFrom(new InternetAddress("cosmos@tid.es"));
            msg.addRecipient(Message.RecipientType.TO,
                             new InternetAddress("cosmos@tid.es"));
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
            int jobId = JobRunnerPool.startNewThread(
                    new String[] { jarPath, HDFS_URL + "/" + inputPath,
                                   HDFS_URL + "/" + outputPath, mongoUrl });
            // TODO: Replace the jobId with an integer, not a string
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
            return JobRunnerPool.getStatus(Integer.parseInt(jobId));
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TransferException(ClusterErrorCode.INVALID_JOB_ID,
                                        ex.getMessage());
        }
    }
    
    private static class JobRunnerPool {
        private static List<JobRunnerThread> threads =
                new ArrayList<JobRunnerThread>();
        
        private JobRunnerPool() {
        }
        
        public static int startNewThread(String[] args) {
            JobRunnerThread thread = new JobRunnerThread(args);
            threads.add(thread);
            thread.start();
            return threads.indexOf(thread);
        }
        
        public static ClusterJobStatus getStatus(int id) {
            JobRunnerThread thread = threads.get(id);
            if (thread.isAlive()) {
                return ClusterJobStatus.RUNNING;
            } else {
                try {
                    thread.join(500);
                } catch (InterruptedException ex) {
                }
                return thread.getStatus();
            }
        }
    }
    
    private static class JobRunnerThread extends Thread {
        private ClusterJobStatus status;
        private String[] args;
        
        public JobRunnerThread(String[] args) {
            this.status = ClusterJobStatus.RUNNING;
            this.args = args.clone();
        }
 
        public ClusterJobStatus getStatus() {
            return this.status;
        }
        
        @Override
        public void run() {
            try {
                RunJar.main(this.args);
                this.status = ClusterJobStatus.SUCCESSFUL;
            } catch (ExitWithSuccessCodeException ex) {
                this.status = ClusterJobStatus.SUCCESSFUL;
            } catch (ExitWithFailureCodeException ex) {
                this.status = ClusterJobStatus.FAILED;
            } catch (Throwable ex) {
                this.status = ClusterJobStatus.FAILED;
            }
        }
    }
}
