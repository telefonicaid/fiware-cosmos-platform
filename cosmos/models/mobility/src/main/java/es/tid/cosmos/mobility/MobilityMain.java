package es.tid.cosmos.mobility;

import java.io.IOException;
import es.tid.cosmos.mobility.jobs.*;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.mapreduce.FilterCellnoinfoMapper;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    // TODO: don't use hard-coded paths. Use properties file instead
    private static final String WORKING_DIRECTORY = "/home/hdfs/mobility";
    private static final String CDRS_MOB_PATH =
            WORKING_DIRECTORY + "/cdrs_mob";
    private static final String CELLS_MOB_PATH =
            WORKING_DIRECTORY + "/cells_mob";
    private static final String CDRS_SAMPLE_PATH =
            WORKING_DIRECTORY + "/cdrs_sample";
    private static final String CDRS_INFO_SMP_PATH =
            WORKING_DIRECTORY + "/cdrs_info_smp";
    private static final String CDRS_NO_INFO_SMP_PATH =
            WORKING_DIRECTORY + "/cdrs_no_info_smp";
    
    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 2) {
            throw new IllegalArgumentException("Mandatory parameters: "
                                               + "cdrs_path cells_path");
        }

        Path cdrsPath = new Path(args[0]);
        Path cellsPath = new Path(args[1]);
        
        this.runParsingJobs(cdrsPath, cellsPath);
        this.runGetVectorsToClusterJobs();

        return 0;
    }
    
    private void extractPointsOfInterest() throws Exception {
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(this.getConf());
            job.configure(null, null);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run NodeBtsCounterJob");
            }
        }
        
        {
            NodeMobInfoJob job = new NodeMobInfoJob(this.getConf());
            job.configure(null, null);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run NodeMobInfoJob");
            }
        }
        
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(
                    this.getConf());
            job.configure(null, null);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run RepbtsSpreadNodebtsJob");
            }
        }
        
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(this.getConf());
            job.configure(null, null);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run RepbtsAggbybtsJob");
            }
        }

        {
        }

        {
        }

        {
            RepbtsGetRepresentativeBtsJob job =
                    new RepbtsGetRepresentativeBtsJob(this.getConf());
            job.configure(null, null);
            if (!job.waitForCompletion(true)) {
                throw new Exception(
                        "Failed to run RepbtsGetRepresentativeBtsJob");
            }
        }
    }
    
    private void runParsingJobs(Path cdrsPath, Path cellsPath)
            throws Exception {
        ParseCdrsJob parseCdrsJob = new ParseCdrsJob(this.getConf());
        parseCdrsJob.configure(cdrsPath, new Path(CDRS_MOB_PATH));
        if (!parseCdrsJob.waitForCompletion(true)) {
            throw new Exception("Failed to parse CDRs");
        }
        
        ParseCellsJob parseCellsJob = new ParseCellsJob(this.getConf());
        parseCellsJob.configure(cellsPath, new Path(CELLS_MOB_PATH));
        if (!parseCellsJob.waitForCompletion(true)) {
            throw new Exception("Failed to parse cells");
        }
    }
    
    private void runGetVectorsToClusterJobs() throws Exception {
        GetSample10000Job getSample10000Job = new GetSample10000Job(
                this.getConf());
        getSample10000Job.configure(new Path(CDRS_MOB_PATH),
                                    new Path(CDRS_SAMPLE_PATH));
        if (!getSample10000Job.waitForCompletion(true)) {
            throw new Exception("Failed to get samples");
        }
        
        // TODO: verify that the keys map to the output files correctly.
        this.getConf().setStrings(CDRS_MOB_PATH, "outputKey", 
                FilterCellnoinfoMapper.OutputKey.CELL.toString());
        FilterCellnoinfoJob filterCellnoinfoJob = new FilterCellnoinfoJob(
                this.getConf());
        filterCellnoinfoJob.configure(new Path(CDRS_SAMPLE_PATH),
                                      new Path(CDRS_INFO_SMP_PATH));
        if (!filterCellnoinfoJob.waitForCompletion(true)) {
            throw new Exception("Failed to filter cells with no info for cells");
        }

        // TODO: verify that the keys map to the output files correctly.
        this.getConf().setStrings(CDRS_MOB_PATH, "outputKey", 
                FilterCellnoinfoMapper.OutputKey.NODE.toString());
        filterCellnoinfoJob = new FilterCellnoinfoJob(this.getConf());
        filterCellnoinfoJob.configure(new Path(CDRS_SAMPLE_PATH),
                                      new Path(CDRS_NO_INFO_SMP_PATH));
        if (!filterCellnoinfoJob.waitForCompletion(true)) {
            throw new Exception("Failed to filter cells with no info for nodes");
        }
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new MobilityMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOG.fatal(ex);
            throw ex;
        }
    }
}
