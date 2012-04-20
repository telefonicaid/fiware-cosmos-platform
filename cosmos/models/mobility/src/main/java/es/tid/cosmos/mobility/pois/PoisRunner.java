package es.tid.cosmos.mobility.pois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.ConvertCdrToMobDataJob;
import es.tid.cosmos.mobility.util.ConvertIntToMobDataJob;
import es.tid.cosmos.mobility.util.ConvertNodeBtsDayToMobDataJob;

/**
 *
 * @author dmicol
 */
public final class PoisRunner {
    private PoisRunner() {
    }
    
    public static void run(Path tmpDir, Path clientsBtsPath,
                           Path clientsInfoPath, Path cdrsNoinfoPath,
                           Path cdrsNoBtsPath, Path clientsInfoFilteredPath,
                           Path clientsRepbtsPath, boolean isDebug,
                           Configuration conf) throws Exception {
        Path nodeBtsCounter = new Path(tmpDir, "node_bts_counter");
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(conf);
            job.configure(clientsBtsPath, nodeBtsCounter);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            NodeMobInfoJob job = new NodeMobInfoJob(conf);
            job.configure(nodeBtsCounter, clientsInfoPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsSpreadNodebts = new Path(tmpDir, "repbts_spread_nodebts");
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(conf);
            job.configure(clientsInfoPath, repbtsSpreadNodebts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsAggbybtsPath = new Path(tmpDir, "repbts_aggbybts");
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(conf);
            job.configure(repbtsSpreadNodebts, repbtsAggbybtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path repbtsAggbybtsMobDataPath = new Path(tmpDir,
                                                  "repbts_aggbybts_mob_data");
        {
            ConvertNodeBtsDayToMobDataJob job =
                    new ConvertNodeBtsDayToMobDataJob(conf);
            job.configure(repbtsAggbybtsPath, repbtsAggbybtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoinfoMobDataPath = new Path(tmpDir, "cdrs_noinfo_mob_data");
        { 
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsNoinfoPath, cdrsNoinfoMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoBtsMobDataPath = new Path(tmpDir, "cdrs_nobts_mob_data");
        { 
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsNoBtsPath, cdrsNoBtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            RepbtsFilterNumCommsJob job = new RepbtsFilterNumCommsJob(conf);
            job.configure(new Path[] { repbtsAggbybtsMobDataPath,
                                       cdrsNoinfoMobDataPath,
                                       cdrsNoBtsMobDataPath },
                          clientsInfoFilteredPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsInfoFilteredMobDataPath = new Path(tmpDir,
                "clients_info_filtered_mob_data");
        {
            ConvertIntToMobDataJob job = new ConvertIntToMobDataJob(conf);
            job.configure(clientsInfoFilteredPath,
                          clientsInfoFilteredMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path repbtsJoinDistComms = new Path(tmpDir, "repbts_join_dist_comms");
        {
            RepbtsJoinDistCommsJob job = new RepbtsJoinDistCommsJob(conf);
            job.configure(new Path[] { repbtsAggbybtsMobDataPath,
                                       clientsInfoFilteredMobDataPath },
                          repbtsJoinDistComms);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            RepbtsGetRepresentativeBtsJob job =
                    new RepbtsGetRepresentativeBtsJob(conf);
            job.configure(repbtsJoinDistComms, clientsRepbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path clientsRepbtsTextPath = new Path(tmpDir, "clients_repbts_text");
            {
                ExportRepresentativeBtsToTextJob job = new
                        ExportRepresentativeBtsToTextJob(conf);
                job.configure(clientsRepbtsPath, clientsRepbtsTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        }
    }
}
