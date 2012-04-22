package es.tid.cosmos.mobility.pois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
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
    
    public static void run(Path tmpDirPath, Path clientsBtsPath,
                           Path clientsInfoPath, Path cdrsNoinfoPath,
                           Path cdrsNoBtsPath, Path clientsInfoFilteredPath,
                           Path clientsRepbtsPath, boolean isDebug,
                           Configuration conf) throws Exception {
        Path clientsBtscounterPath = new Path(tmpDirPath, "clients_btscounter");
        {
            NodeBtsCounterJob job = new NodeBtsCounterJob(conf);
            job.configure(clientsBtsPath, clientsBtscounterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            NodeMobInfoJob job = new NodeMobInfoJob(conf);
            job.configure(clientsBtscounterPath, clientsInfoPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsInfoSpreadPath = new Path(tmpDirPath, "clients_info_spread");
        {
            RepbtsSpreadNodebtsJob job = new RepbtsSpreadNodebtsJob(conf);
            job.configure(clientsInfoPath, clientsInfoSpreadPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsInfoAggbybtsPath = new Path(tmpDirPath,
                                                "clients_info_aggbybts");
        {
            RepbtsAggbybtsJob job = new RepbtsAggbybtsJob(conf);
            job.configure(clientsInfoSpreadPath, clientsInfoAggbybtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path repbtsAggbybtsMobDataPath = new Path(tmpDirPath,
                                                  "repbts_aggbybts_mob_data");
        {
            ConvertNodeBtsDayToMobDataJob job =
                    new ConvertNodeBtsDayToMobDataJob(conf);
            job.configure(clientsInfoAggbybtsPath, repbtsAggbybtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoinfoMobDataPath = new Path(tmpDirPath,
                                              "cdrs_noinfo_mob_data");
        { 
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsNoinfoPath, cdrsNoinfoMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path cdrsNoBtsMobDataPath = new Path(tmpDirPath, "cdrs_nobts_mob_data");
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

        Path clientsInfoFilteredMobDataPath = new Path(tmpDirPath,
                "clients_info_filtered_mob_data");
        {
            ConvertIntToMobDataJob job = new ConvertIntToMobDataJob(conf);
            job.configure(clientsInfoFilteredPath,
                          clientsInfoFilteredMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientsInfoBtsPercPath = new Path(tmpDirPath,
                                               "clients_info_bts_perc");
        {
            RepbtsJoinDistCommsJob job = new RepbtsJoinDistCommsJob(conf);
            job.configure(new Path[] { repbtsAggbybtsMobDataPath,
                                       clientsInfoFilteredMobDataPath },
                          clientsInfoBtsPercPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            RepbtsGetRepresentativeBtsJob job =
                    new RepbtsGetRepresentativeBtsJob(conf);
            job.configure(clientsInfoBtsPercPath, clientsRepbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path clientsRepbtsTextPath = new Path(tmpDirPath,
                                                  "clients_repbts_text");
            {
                ExportRepresentativeBtsToTextJob job = new
                        ExportRepresentativeBtsToTextJob(conf);
                job.configure(clientsRepbtsPath, clientsRepbtsTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        } else {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(clientsBtsPath, true);
            fs.delete(clientsBtscounterPath, true);
            fs.delete(clientsInfoSpreadPath, true);
            fs.delete(clientsInfoAggbybtsPath, true);
            fs.delete(clientsInfoBtsPercPath, true);
            fs.delete(cdrsNoinfoPath, true);
            fs.delete(cdrsNoBtsPath, true);
        }
    }
}
