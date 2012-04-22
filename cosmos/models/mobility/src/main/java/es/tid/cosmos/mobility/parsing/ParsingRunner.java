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

    public static void run(Path cdrsPath, Path cdrsMobPath,
                           Path cellsPath, Path cellsMobPath,
                           Path pairbtsadjTxtPath, Path pairbtsAdjPath,
                           Path btsVectorTxtPath, Path btsComareaPath,
                           Configuration conf) throws Exception {
        {
            ParseCdrsJob job = new ParseCdrsJob(conf);
            job.configure(cdrsPath, cdrsMobPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            ParseCellsJob job = new ParseCellsJob(conf);
            job.configure(cellsPath, cellsMobPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            AdjParseAdjBtsJob job = new AdjParseAdjBtsJob(conf);
            job.configure(pairbtsadjTxtPath, pairbtsAdjPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            BorrarGetBtsComareaJob job = new BorrarGetBtsComareaJob(conf);
            job.configure(btsVectorTxtPath, btsComareaPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
