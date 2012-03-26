package es.tid.bdp.hadoopjobs.mapperfail;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.Tool;

/**
 * @author ximo
 *
 */
public class MapperFailTool extends Configured implements Tool
{
    @Override
    public int run(String[] strings) throws Exception {
        MapperFailJob testJob = new MapperFailJob(this.getConf());
        if (!testJob.waitForCompletion(true)) {
            return 1;
        }
        return 0;
    }
}
