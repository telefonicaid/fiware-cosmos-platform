package es.tid.bdp.hadoopjobs.printprimes;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.Tool;

/**
 * @author ximo
 *
 */
public class PrimePrintTool extends Configured implements Tool {
    @Override
    public int run(String[] strings) throws Exception {
        PrimePrintJob testJob = new PrimePrintJob(this.getConf());
        if (!testJob.waitForCompletion(true)) {
            return 1;
        }
        return 0;
    }
}
