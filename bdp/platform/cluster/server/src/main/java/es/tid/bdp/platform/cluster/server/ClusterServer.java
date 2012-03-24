package es.tid.bdp.platform.cluster.server;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
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

    private void start() throws Throwable {
        ClusterServer cluster = new ClusterServer();
        TServerSocket serverTransport = new TServerSocket(9888);
        Cluster.Processor processor = new Cluster.Processor(cluster);
        Args args = new Args(serverTransport);
        args.processor(processor);
        TServer server = new TThreadPoolServer(args);
        server.serve();
    }

    @Override
    public void copyToHdfs(String src, String dest) throws TException {
        try {
            Configuration config = new Configuration();
            config.set("fs.default.name", HDFS_URL);
            FileSystem fs = FileSystem.get(config);
            fs.moveFromLocalFile(new Path(src), new Path(dest));
        } catch (Exception ex) {
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
            throw new TException(ex);
        }
    }
}
