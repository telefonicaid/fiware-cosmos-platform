package es.tid.cosmos.hadoopjobs.mapperfail;

import java.security.InvalidParameterException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

/**
 * @author ximo
 *
 */
public class MapperFailTool extends Configured implements Tool {
    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 3) {
            throw new InvalidParameterException("Expecting 3 arguments."
                    + " Received: " + args.length);
        }
        MapperFailJob testJob = new MapperFailJob(this.getConf());        
        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1]);
        testJob.configure(inputPath, outputPath);        
        if (!testJob.waitForCompletion(true)) {
            return 1;
        }
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
