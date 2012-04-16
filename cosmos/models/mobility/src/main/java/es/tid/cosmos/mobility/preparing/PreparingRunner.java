package es.tid.cosmos.mobility.preparing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class PreparingRunner {
    private PreparingRunner() {
    }

    public static void run(Path cdrsMobPath, Path cdrsInfoPath,
                           Path cdrsNoinfoPath, Path cellsMobPath,
                           Path clientsBtsPath, Path btsCommsPath,
                           Path cdrsNoBtsPath, Path viTelmonthBtsPath,
                           Configuration conf) throws Exception {
        {
            FilterCellnoinfoByCellIdJob job = new FilterCellnoinfoByCellIdJob(
                    conf);
            job.configure(cdrsMobPath, cdrsInfoPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            FilterCellnoinfoByNodeIdJob job = new FilterCellnoinfoByNodeIdJob(
                    conf);
            job.configure(cdrsMobPath, cdrsNoinfoPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            JoinBtsNodeToBtsDayRangeJob job = new JoinBtsNodeToBtsDayRangeJob(
                    conf);
            job.configure(new Path[] { cdrsInfoPath, cellsMobPath },
                          clientsBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToCdrJob job = new JoinBtsNodeToCdrJob(conf);
            job.configure(new Path[] { cdrsInfoPath, cellsMobPath },
                          btsCommsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToNodeBtsJob job = new JoinBtsNodeToNodeBtsJob(conf);
            job.configure(new Path[] { cdrsInfoPath, cellsMobPath },
                          cdrsNoBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToTelMonthAndCellJob job =
                    new JoinBtsNodeToTelMonthAndCellJob(conf);
            job.configure(new Path[] { cdrsInfoPath, cellsMobPath },
                          viTelmonthBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
