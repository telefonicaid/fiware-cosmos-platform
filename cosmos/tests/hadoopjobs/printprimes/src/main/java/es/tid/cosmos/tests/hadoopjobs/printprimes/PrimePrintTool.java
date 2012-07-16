package es.tid.cosmos.tests.hadoopjobs.printprimes;

import java.io.IOException;
import java.security.InvalidParameterException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.mapreduce.CosmosJob;

/**
 * @author ximo
 *
 */
public class PrimePrintTool extends Configured implements Tool {
    @Override
    public int run(String[] args)
            throws IOException, InterruptedException, ClassNotFoundException {
        if (args.length != 3) {
            throw new InvalidParameterException("Expecting 3 arguments."
                    + " Received: " + args.length);
        }
        CosmosJob testJob = CosmosJob.createMapReduceJob(
                this.getConf(), "TestJar", TextInputFormat.class,
                PrimePrintMapper.class, PrimePrintReducer.class,
                MongoOutputFormat.class);
        FileInputFormat.setInputPaths(testJob, new Path(args[0]));
        MongoConfigUtil.setOutputURI(testJob.getConfiguration(), args[2]);
        testJob.waitForCompletion(true);
        return 0;
    }

    public static void main(String[] args) throws Exception {
        int ret = ToolRunner.run(new Configuration(),
                       new PrimePrintTool(), args);
        if (ret != 0) {
            throw new Exception("Something bad happened");
        }
    }
}
