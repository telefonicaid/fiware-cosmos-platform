package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
public class FileDataEraser extends DataEraser {
    @Override
    public void deleteInputs(Job job) throws IOException {
        FileSystem fs = FileSystem.get(job.getConfiguration());
        Path[] inputPaths = FileInputFormat.getInputPaths(job);
        for (Path path : inputPaths) {
            if (!fs.deleteOnExit(path)) {
                throw new IOException("Failed to delete input path "
                        + path.toString());
            }
        }
    }

    @Override
    public void deleteOutput(Job job) throws IOException {
        FileSystem fs = FileSystem.get(job.getConfiguration());
        Path outputPaths = FileOutputFormat.getOutputPath(job);
        if (!fs.deleteOnExit(outputPaths)) {
            throw new IOException("Failed to delete output path "
                    + outputPaths.toString());
        }
    }
}
