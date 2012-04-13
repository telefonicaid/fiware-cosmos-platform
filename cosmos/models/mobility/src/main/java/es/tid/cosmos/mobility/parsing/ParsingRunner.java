package es.tid.cosmos.mobility.parsing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.jobs.ParseCdrsJob;
import es.tid.cosmos.mobility.jobs.ParseCellsJob;

/**
 *
 * @author dmicol
 */
public final class ParsingRunner {
    private ParsingRunner() {
    }

    public static void run(Path cdrsPath, Path cellsPath, Configuration conf)
            throws Exception {
        Path parseCdrs = cdrsPath.suffix("parse_cdrs");
        {
            ParseCdrsJob job = new ParseCdrsJob(conf);
            job.configure(cdrsPath, parseCdrs);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path parseCells = cellsPath.suffix("parse_cells");
        {
            ParseCellsJob job = new ParseCellsJob(conf);
            job.configure(cellsPath, parseCells);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
