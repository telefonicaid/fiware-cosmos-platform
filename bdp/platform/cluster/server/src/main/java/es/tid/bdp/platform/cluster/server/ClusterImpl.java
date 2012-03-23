package es.tid.bdp.platform.cluster.server;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.RunJar;
import org.apache.thrift.TException;

/**
 *
 * @author dmicol
 */
public class ClusterImpl implements Cluster.Iface {
    // TODO: put this in a configuration file
    private static final String HDFS_URL = "hdfs://pshdp01:8011";

    @Override
    public void copyToHdfs(String src, String dest) throws TException {
        try {
            Configuration config = new Configuration();
            config.set("fs.default.name", HDFS_URL);
            FileSystem fs = FileSystem.get(config);

            Path originPath = new Path(src);
            Path destinationPath = new Path(dest);
            fs.moveFromLocalFile(originPath, destinationPath);
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
