package es.tid.bdp.hadoopjobs.mapperfail;

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
        MapperFailJob testJob = new MapperFailJob(this.getConf());
        
        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1]);
        testJob.configure(inputPath, outputPath);
        
        if (!testJob.waitForCompletion(true)) {
            throw new Exception("Something bad happened");
        }
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        ToolRunner.run(new Configuration(),
                    new MapperFailTool(), args);
    }
}
