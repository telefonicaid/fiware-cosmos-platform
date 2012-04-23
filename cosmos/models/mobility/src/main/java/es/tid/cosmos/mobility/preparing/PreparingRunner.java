package es.tid.cosmos.mobility.preparing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.ConvertCdrToMobDataJob;
import es.tid.cosmos.mobility.util.ConvertCellToMobDataJob;

/**
 *
 * @author dmicol
 */
public final class PreparingRunner {
    private PreparingRunner() {
    }

    public static void run(Path tmpPath, Path cdrsMobPath, Path cdrsInfoPath,
                           Path cdrsNoinfoPath, Path cellsMobPath,
                           Path clientsBtsPath, Path btsCommsPath,
                           Path cdrsNoBtsPath, Path viTelmonthBtsPath,
                           Configuration conf) throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
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

        Path cdrsInfoMobDataPath = tmpPath.suffix("/cdrs_info_mob_data");
        {
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsInfoPath, cdrsInfoMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path cellsMobDataPath = tmpPath.suffix("/cells_mob_data");
        {
            ConvertCellToMobDataJob job = new ConvertCellToMobDataJob(conf);
            job.configure(cellsMobPath, cellsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToNodeBtsJob job = new JoinBtsNodeToNodeBtsJob(conf);
            job.configure(new Path[] { cdrsInfoMobDataPath, cellsMobDataPath },
                          clientsBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToBtsDayRangeJob job = new JoinBtsNodeToBtsDayRangeJob(
                    conf);
            job.configure(new Path[] { cdrsInfoMobDataPath, cellsMobDataPath },
                          btsCommsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToCdrJob job = new JoinBtsNodeToCdrJob(conf);
            job.configure(new Path[] { cdrsInfoMobDataPath, cellsMobDataPath },
                          cdrsNoBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            JoinBtsNodeToTelMonthAndCellJob job =
                    new JoinBtsNodeToTelMonthAndCellJob(conf);
            job.configure(new Path[] { cdrsInfoMobDataPath, cellsMobDataPath },
                          viTelmonthBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(cdrsInfoMobDataPath, true);
        fs.delete(cellsMobDataPath, true);
    }
}
