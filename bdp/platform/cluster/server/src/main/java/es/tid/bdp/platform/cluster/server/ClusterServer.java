package es.tid.bdp.platform.cluster.server;

import java.io.IOException;
import java.util.LinkedList;
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
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.JobStatus;
import org.apache.hadoop.util.RunJar;
import org.apache.thrift.TException;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.server.TThreadPoolServer.Args;
import org.apache.thrift.transport.TServerSocket;

/**
 *
 * @author dmicol
 */
public class ClusterServer implements Cluster.Iface {
    private enum JobsList {
        ALL,
        RUNNING
    }
    
    private Configuration conf;
    // TODO: put this in a configuration file
    private static final String HDFS_URL = "hdfs://pshdp01:8011";

    public static void main(String args[]) {
        try {
            ClusterServer server = new ClusterServer();
            server.start();
        } catch (Throwable ex) {
            System.exit(1);
        }
    }

    public ClusterServer() {
        this.conf = new Configuration();
        // TODO: this might not be necessary
        this.conf.set("fs.default.name", HDFS_URL);
    }

    private void start() throws Throwable {
        ClusterServer cluster = new ClusterServer();
        TServerSocket serverTransport = new TServerSocket(9888);
        Cluster.Processor processor = new Cluster.Processor(cluster);
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
    public void copyToHdfs(String src, String dest) throws TException {
        try {
            FileSystem fs = FileSystem.get(this.conf);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }

    @Override
    public void runJob(String jarPath, String inputPath, String outputPath,
                       String mongoUrl) throws TException {
        try {
            // TODO: this should be done using Jobs, to get tracking info
            RunJar.main(new String[] { jarPath, inputPath,
                                       outputPath, mongoUrl });
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }

    @Override
    public List<ClusterJobStatus> getAllJobs(String user)
            throws TransferException, TException {
        try {
            return this.getJobs(user, JobsList.ALL);
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }
    
    @Override
    public List<ClusterJobStatus> getRunningJobs(String user)
            throws TransferException, TException {
        try {
            return this.getJobs(user, JobsList.RUNNING);
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }

    private List<ClusterJobStatus> getJobs(String user, JobsList jobsToFetch)
            throws IOException {
        JobClient client = new JobClient(new JobConf(this.conf));
        JobStatus[] jobs;
        switch (jobsToFetch) {
            case ALL:
                jobs = client.getAllJobs();
                break;
            case RUNNING:
                jobs = client.jobsToComplete();
                break;
            default:
                throw new IllegalStateException("Invalid setting");
        }

        List<ClusterJobStatus> statuses =
                new LinkedList<ClusterJobStatus>();
        for (JobStatus job : jobs) {
            ClusterJobStatus status = new ClusterJobStatus();
            status.setId(job.getJobID().getId());
            status.setUsername(job.getUsername());
        }
        return statuses;
    }
}
