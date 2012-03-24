package es.tid.bdp.platform.cluster.server;

import java.util.Properties;

import javax.mail.Message;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.*;
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
    private static final String JOBTRACKER_URL = "pshdp01:8012";

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
        this.conf.set("mapred.job.tracker", JOBTRACKER_URL);
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
    public String runJob(String jarPath, String inputPath, String outputPath,
                         String mongoUrl) throws TException {
        Path input = new Path(inputPath);
        Path output = new Path(outputPath);
        
        try {
            FileSystem fs = FileSystem.get(this.conf);
            if (fs.exists(output)) {
                fs.delete(output, true);
            }
            
            JobConf jobConf = new JobConf(this.conf);
            jobConf.setJar(jarPath);
            FileInputFormat.setInputPaths(jobConf, input);
            FileOutputFormat.setOutputPath(jobConf, output);
            JobClient client = new JobClient(jobConf);
            RunningJob runInfo = client.submitJob(jobConf);
            return runInfo.getID().getJtIdentifier();
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }

    @Override
    public ClusterJobStatus getJobStatus(String jobId) throws TException {
        try {
            JobClient client = new JobClient(new JobConf(this.conf));
            for (org.apache.hadoop.mapred.JobStatus jobStatus
                    : client.getAllJobs()) {
                if (jobId.equals(jobStatus.getJobID().getJtIdentifier())) {
                    return ClusterJobStatus.findByValue(
                            jobStatus.getRunState());
                }
            }
            throw new IllegalArgumentException("Invalid job ID");
        } catch (Throwable ex) {
            this.sendNotificationEmail(ex.getMessage(), ex.toString());
            throw new TException(ex);
        }
    }
}
