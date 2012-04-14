package es.tid.cosmos.mobility.parsing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;


/**
 *
 * @author dmicol
 */
public final class ParsingRunner {
    private ParsingRunner() {
    }

    public static void run(Path cdrsPath, Path parsedCdrsPath,
                           Path cellsPath, Path parsedCellsPath,
                           Path adjBtsPath, Path parsedAdjBtsPath,
                           Path btsComarea, Path parsedBtsComarea,
                           Configuration conf) throws Exception {
        {
            ParseCdrsJob job = new ParseCdrsJob(conf);
            job.configure(cdrsPath, parsedCdrsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            ParseCellsJob job = new ParseCellsJob(conf);
            job.configure(cellsPath, parsedCellsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            AdjParseAdjBtsJob job = new AdjParseAdjBtsJob(conf);
            job.configure(adjBtsPath, parsedAdjBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            BorrarGetBtsComareaJob job = new BorrarGetBtsComareaJob(conf);
            job.configure(btsComarea, parsedBtsComarea);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
