package es.tid.cosmos.tests.hadoopjobs.mapperfail;

import java.io.IOException;
import java.security.InvalidParameterException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.mapreduce.CosmosJob;

/**
 * @author ximo
 *
 */
public class MapperFailTool extends Configured implements Tool {
    @Override
    public int run(String[] args)
            throws IOException, InterruptedException, ClassNotFoundException {
        if (args.length != 3) {
            throw new InvalidParameterException("Expecting 3 arguments."
                    + " Received: " + args.length);
        }
        CosmosJob testJob = CosmosJob.createMapJob(
                this.getConf(), "TestJar", TextInputFormat.class,
                MapperFailMapper.class, TextOutputFormat.class);
        FileInputFormat.setInputPaths(testJob, new Path(args[0]));
        FileOutputFormat.setOutputPath(testJob, new Path(args[1]));
        testJob.waitForCompletion(true);
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        int ret = ToolRunner.run(new Configuration(),
                    new MapperFailTool(), args);
        if (ret != 0) {
            throw new Exception("Something bad happened");
        }
    }
}
