package es.tid.cosmos.tests.hadoopjobs.parameterizedjob.base;

import java.io.IOException;

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
public class ParameterizedJobTool extends Configured implements Tool {

    @Override
    public int run(String[] args)
            throws IOException, InterruptedException, ClassNotFoundException {
        if (args.length != 0) {
            throw new IllegalArgumentException("The tool should not be receiving"
                    + " any parameters.");
        }
        final Configuration config = this.getConf();
        CosmosJob testJob = CosmosJob.createMapReduceJob(
                config, "ParameterizedTestJar", TextInputFormat.class,
                MultiplePrintMapper.class, MultiplePrintReducer.class,
                MongoOutputFormat.class);
        FileInputFormat.setInputPaths(testJob,
                                      new Path(config.get("cosmos.test.input")));
        MongoConfigUtil.setOutputURI(testJob.getConfiguration(),
                                     config.get("cosmos.test.mongo_output"));
        testJob.waitForCompletion(true);
        return 0;
    }

    public static void main(String[] args) {
        try {
            int ret = ToolRunner.run(new Configuration(),
                        new ParameterizedJobTool(), args);
            if (ret != 0) {
                throw new IllegalStateException("Non-zero return code: "
                        + Integer.toString(ret));
            }
        } catch(Exception ex) {
            throw new IllegalStateException("Something bad happened", ex);
        }
    }
}
