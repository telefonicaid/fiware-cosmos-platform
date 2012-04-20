package es.tid.cosmos.mobility.labelling.secondhomes;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.*;


/**
 *
 * @author dmicol
 */
public final class DetectSecondHomesRunner {
    private DetectSecondHomesRunner() {
    }
    
    public static void run(Path cellsMobPath, Path pointsOfInterestTemp4Path,
                           Path viClientFuseAccPath, Path pairbtsAdjPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws Exception {
        Path btsMobPath = new Path(tmpDirPath, "bts_mob");
        {
            PoiCellToBtsJob job = new PoiCellToBtsJob(conf);
            job.configure(cellsMobPath, btsMobPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path pointsOfInterestTemp4MobDataPath = new Path(tmpDirPath,
                "points_of_interest_temp4_mob_data");
        {
            ConvertPoiToMobDataJob job = new ConvertPoiToMobDataJob(conf);
            job.configure(pointsOfInterestTemp4Path,
                          pointsOfInterestTemp4MobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path btsMobMobDataPath = new Path(tmpDirPath, "bts_mob_mob_data");
        {
            ConvertCellToMobDataJob job = new ConvertCellToMobDataJob(conf);
            job.configure(btsMobPath, btsMobMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path sechPoiPosPath = new Path(tmpDirPath, "sech_poi_pos");
        {
            PoiJoinPoisBtscoordToPoiPosJob job =
                    new PoiJoinPoisBtscoordToPoiPosJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp4MobDataPath,
                                       btsMobMobDataPath },
                          sechPoiPosPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodbtsPoiPath = new Path(tmpDirPath, "nodbts_poi");
        {
            PoiJoinPoisBtscoordToPoiJob job = new PoiJoinPoisBtscoordToPoiJob(
                    conf);
            job.configure(new Path[] { pointsOfInterestTemp4MobDataPath,
                                       btsMobMobDataPath },
                          nodbtsPoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path sechPoiPosMobDataPath = new Path(tmpDirPath, "bts_mob_mob_data");
        {
            ConvertPoiPosToMobDataJob job = new ConvertPoiPosToMobDataJob(conf);
            job.configure(sechPoiPosPath, sechPoiPosMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path viClientFuseAccMobDataPath = new Path(tmpDirPath,
                "vi_client_fuse_acc_mob_data");
        {
            ConvertMobViMobVarsToMobDataJob job =
                    new ConvertMobViMobVarsToMobDataJob(conf);
            job.configure(viClientFuseAccPath, viClientFuseAccMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path sechPoiInoutPath = new Path(tmpDirPath, "vector_bts");
        {
            PoiJoinPoisViToPoiPosJob job = new PoiJoinPoisViToPoiPosJob(conf);
            job.configure(new Path[] { sechPoiPosMobDataPath,
                                       viClientFuseAccMobDataPath },
                          sechPoiInoutPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodbtsInoutPath = new Path(tmpDirPath, "nodbts_inout");
        {
            PoiJoinPoisViToTwoIntJob job = new PoiJoinPoisViToTwoIntJob(conf);
            job.configure(new Path[] { sechPoiPosMobDataPath,
                                       viClientFuseAccMobDataPath },
                          nodbtsInoutPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path sechPotSecHomePath = new Path(tmpDirPath, "sech_pot_sec_home");
        {
            GetPairsSechomePoisJob job = new GetPairsSechomePoisJob(conf);
            job.configure(sechPoiInoutPath, sechPotSecHomePath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path sechPotSecHomeMobDataPath = new Path(tmpDirPath,
                                                  "sech_pot_sec_home_mob_data");
        {
            ConvertLongToMobDataByTwoIntJob job =
                    new ConvertLongToMobDataByTwoIntJob(conf);
            job.configure(sechPotSecHomePath, sechPotSecHomeMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pairsbtsAdjMobDataPath = new Path(tmpDirPath,
                                               "pairbts_adj_mob_data");
        {
            ConvertLongToMobDataByTwoIntJob job =
                    new ConvertLongToMobDataByTwoIntJob(conf);
            job.configure(pairbtsAdjPath, pairsbtsAdjMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path nodbtsSechomePath = new Path(tmpDirPath, "nodbts_sechome");
        {
            PoiFilterSechomeAdjacentJob job = new PoiFilterSechomeAdjacentJob(
                    conf);
            job.configure(new Path[] { sechPotSecHomeMobDataPath,
                                       pairsbtsAdjMobDataPath },
                          nodbtsSechomePath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path nodbtsSechomeUniqPath = new Path(tmpDirPath, "nodbts_sechome_uniq");
        {
            PoiDeleteSechomeDuplicateJob job =
                    new PoiDeleteSechomeDuplicateJob(conf);
            job.configure(new Path[] { sechPotSecHomeMobDataPath,
                                       pairsbtsAdjMobDataPath },
                          nodbtsSechomeUniqPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodbtsPoiMobDataPath = new Path(tmpDirPath, "nodbts_poi_mob_data");
        {
            ConvertPoiToMobDataByTwoIntJob job =
                    new ConvertPoiToMobDataByTwoIntJob(conf);
            job.configure(pairbtsAdjPath, pairsbtsAdjMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path nodbtsInoutMobDataPath = new Path(tmpDirPath,
                                               "nodbts_inout_mob_data");
        {
            ConvertTwoIntToMobDataByTwoIntJob job =
                    new ConvertTwoIntToMobDataByTwoIntJob(conf);
            job.configure(nodbtsInoutPath, nodbtsInoutMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path nodbtsSechomeUniqMobDataPath = new Path(tmpDirPath,
                "nodbts_sechome_uniq_mob_data");
        {
            ConvertNullToMobDataByTwoIntJob job =
                    new ConvertNullToMobDataByTwoIntJob(conf);
            job.configure(pairbtsAdjPath, pairsbtsAdjMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestPath = new Path(tmpDirPath, "points_of_interest");
        {
            PoiJoinSechomeResultsJob job = new PoiJoinSechomeResultsJob(conf);
            job.configure(new Path[] { nodbtsPoiMobDataPath,
                                       nodbtsInoutMobDataPath,
                                       nodbtsSechomeUniqMobDataPath },
                          pointsOfInterestPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
