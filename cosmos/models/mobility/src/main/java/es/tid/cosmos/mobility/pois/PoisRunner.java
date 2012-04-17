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
    
    public static void run(Path tmpDir, Path clientsBtsPath, Path cdrsNoinfoPath,
                           Path cdrsNoBtsPath, Path clientsInfoFilteredPath,
                           Path clientsRepbtsPath, Configuration conf)
            throws Exception {
        Path nodeBtsCounter = tmpDir.suffix("/node_bts_counter");
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(conf);
            job.configure(clientsBtsPath, nodeBtsCounter);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodeMobInfo = tmpDir.suffix("/node_mob_info");
        {
            NodeMobInfoJob job = new NodeMobInfoJob(conf);
            job.configure(nodeBtsCounter, nodeMobInfo);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsSpreadNodebts = tmpDir.suffix("/repbts_spread_nodebts");
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(conf);
            job.configure(nodeMobInfo, repbtsSpreadNodebts);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path repbtsAggbybtsPath = tmpDir.suffix("/repbts_aggbybts");
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(conf);
            job.configure(repbtsSpreadNodebts, repbtsAggbybtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path repbtsAggbybtsMobDataPath =
                tmpDir.suffix("/repbts_aggbybts_mob_data");
        {
            ConvertNodeBtsDayToMobDataJob job =
                    new ConvertNodeBtsDayToMobDataJob(conf);
            job.configure(repbtsAggbybtsPath, repbtsAggbybtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoinfoMobDataPath = tmpDir.suffix("/cdrs_noinfo_mob_data");
        { 
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsNoinfoPath, cdrsNoinfoMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoBtsMobDataPath = tmpDir.suffix("/cdrs_nobts_mob_data");
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

        Path clientsInfoFilteredMobDataPath = tmpDir.suffix(
                "/clients_info_filtered_mob_data");
        {
            ConvertIntToMobDataJob job = new ConvertIntToMobDataJob(conf);
            job.configure(clientsInfoFilteredPath,
                          clientsInfoFilteredMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path repbtsJoinDistComms = tmpDir.suffix("/repbts_join_dist_comms");
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

        Path clientsRepbtsTextPath = tmpDir.suffix("/clients_repbts_text");
        {
            RepbtsGetRepresentativeBtsToTextJob job = new
                    RepbtsGetRepresentativeBtsToTextJob(conf);
            job.configure(clientsRepbtsPath, clientsRepbtsTextPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
