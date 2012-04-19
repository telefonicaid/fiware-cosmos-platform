package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
public abstract class CosmosJob extends Job implements Runnable {
    public CosmosJob(Configuration conf, String jobName)
            throws IOException {
        super(conf, jobName);
    }

    @Override
    public final void waitForCompletion(EnumSet<CleanupOptions> options)
            throws Exception {
        if (!this.waitForCompletion(true)) {
            throw new Exception("Failed to run " + this.getJobName());
        }
        FileSystem fs = FileSystem.get(this.getConfiguration());
        if (options.contains(CleanupOptions.DeleteInput)) {
            Path[] paths = FileInputFormat.getInputPaths(this);
            for (Path path : paths) {
                if (!fs.deleteOnExit(path)) {
                    throw new Exception("Failed to delete input path "
                            + path.toString());
                }
            }
        }
        if (options.contains(CleanupOptions.DeleteOutput)) {
            Path path = FileOutputFormat.getOutputPath(this);
            if (!fs.deleteOnExit(path)) {
                throw new Exception("Failed to delete output path "
                        + path.toString());
            }
        }
    }
    
    @Override
    public final List<CosmosJob> getJobs() {
        List<CosmosJob> ret = new ArrayList<CosmosJob>();
        ret.add(this);
        return ret;
    }
}