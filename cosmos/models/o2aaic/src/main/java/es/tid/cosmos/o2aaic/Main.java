package es.tid.cosmos.o2aaic;

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
import es.tid.cosmos.base.util.Logger;

/**
 *
 * @author dmicol
 */
public class Main extends Configured implements Tool {
    @Override
    public int run(String[] args) throws Exception {
        final Path inetRawPath = new Path(args[0]);
        final Path inetIpmPath = new Path(args[1]);
        
        CosmosJob job = CosmosJob.createMapJob(this.getConf(),
                                               "InetRawToIpm",
                                               TextInputFormat.class,
                                               InetRawToIpmMapper.class,
                                               TextOutputFormat.class);
        FileInputFormat.setInputPaths(job, inetRawPath);
        FileOutputFormat.setOutputPath(job, inetIpmPath);
        job.waitForCompletion(true);
        
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new Main(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            Logger.get(Main.class).fatal(ex);
            throw ex;
        }
    }
}
