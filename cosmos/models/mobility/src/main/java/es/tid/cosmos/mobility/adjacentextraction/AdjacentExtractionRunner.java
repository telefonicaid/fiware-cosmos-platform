package es.tid.cosmos.mobility.adjacentextraction;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.*;

/**
 *
 * @author dmicol
 */
public final class AdjacentExtractionRunner {
    private AdjacentExtractionRunner() {
    }
    
    public static void run(Path pointsOfInterestPath, Path pairbtsAdjPath,
                           Path pointsOfInterestIdPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path poisIdPath = new Path(tmpDirPath, "pois_id");
        {
            AdjAddUniqueIdPoiToPoiNewJob job = new AdjAddUniqueIdPoiToPoiNewJob(
                    conf);
            job.configure(pointsOfInterestPath, poisIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poisTablePath = new Path(tmpDirPath, "pois_table");
        {
            AdjAddUniqueIdPoiToTwoIntJob job = new AdjAddUniqueIdPoiToTwoIntJob(
                    conf);
            job.configure(pointsOfInterestPath, poisTablePath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poiPairbtsPath = new Path(tmpDirPath, "poi_pairbts");
        {
            AdjGroupTypePoiClientJob job = new AdjGroupTypePoiClientJob(conf);
            job.configure(poisIdPath, poiPairbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poiPairbtsMobDataPath = new Path(tmpDirPath,
                                              "poi_pairbts_mob_data");
        {
            ConvertTwoIntToMobDataByTwoIntJob job =
                    new ConvertTwoIntToMobDataByTwoIntJob(conf);
            job.configure(poiPairbtsPath, poiPairbtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path pairbtsAdjMobDataPath = new Path(tmpDirPath,
                                              "pairbts_adj_mob_data");
        {
            ConvertNullToMobDataByTwoIntJob job =
                    new ConvertNullToMobDataByTwoIntJob(conf);
            job.configure(pairbtsAdjPath, pairbtsAdjMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path poiPairbtsMobDataWithInputIdPath = new Path(tmpDirPath,
                "poi_pairbts_mob_data_with_input_id");
        {
            SetMobDataInputIdByTwoIntJob job = new SetMobDataInputIdByTwoIntJob(
                    conf);
            job.configure(poiPairbtsMobDataPath, 0,
                          poiPairbtsMobDataWithInputIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pairbtsAdjMobDataWithInputIdPath = new Path(tmpDirPath,
                "pairbts_adj_mob_data_with_input_id");
        {
            SetMobDataInputIdByTwoIntJob job = new SetMobDataInputIdByTwoIntJob(
                    conf);
            job.configure(pairbtsAdjMobDataPath, 1,
                          pairbtsAdjMobDataWithInputIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path poiPairbtsAdjPath = new Path(tmpDirPath, "poi_pairbts_adj");
        {
            AdjJoinPairbtsAdjbtsJob job = new AdjJoinPairbtsAdjbtsJob(conf);
            job.configure(new Path[] { poiPairbtsMobDataWithInputIdPath,
                                       pairbtsAdjMobDataWithInputIdPath },
                          poiPairbtsAdjPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        long numIndicesLeft;
        do {
            Path poiPairbtsIndexPath = new Path(tmpDirPath,
                                                "poi_pairbts_index");
            {
                AdjPutMaxIdJob job = new AdjPutMaxIdJob(conf);
                job.configure(poiPairbtsAdjPath, poiPairbtsIndexPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            Path poiPairbtsIndexMobDataPath = new Path(tmpDirPath,
                    "poi_pairbts_index_mob_data");
            {
                ConvertTwoIntToMobDataJob job = new ConvertTwoIntToMobDataJob(conf);
                job.configure(poiPairbtsIndexPath, poiPairbtsIndexMobDataPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            Path poisTableMobDataPath = new Path(tmpDirPath,
                                                 "pois_table_mob_data");
            {
                ConvertTwoIntToMobDataJob job = new ConvertTwoIntToMobDataJob(
                        conf);
                job.configure(poisTablePath, poisTableMobDataPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            Path poisTableTmpPath = new Path(tmpDirPath, "pois_table_tmp");
            {
                AdjUpdatePoisTableJob job = new AdjUpdatePoisTableJob(conf);
                job.configure(new Path[] { poisTableMobDataPath,
                                           poiPairbtsIndexMobDataPath },
                              poisTableTmpPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            fs.delete(poisTablePath, true);
            fs.delete(poisTableMobDataPath, true);
            fs.rename(poisTableTmpPath, poisTablePath);
            
            Path poiPairbtsAdjMobDataPath = new Path(tmpDirPath,
                                                     "poi_pairbts_adj_mob_data");
            {
                ConvertTwoIntToMobDataJob job = new ConvertTwoIntToMobDataJob(
                        conf);
                job.configure(poiPairbtsAdjPath, poiPairbtsAdjMobDataPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            Path poiPairbtsCh1Path = new Path(tmpDirPath, "poi_pairbts_ch1");
            {
                AdjSwapPoiIdSt1Job job = new AdjSwapPoiIdSt1Job(conf);
                job.configure(new Path[] { poiPairbtsAdjMobDataPath,
                                           poiPairbtsIndexMobDataPath },
                              poiPairbtsCh1Path);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            fs.delete(poiPairbtsAdjPath, true);
            fs.delete(poiPairbtsAdjMobDataPath, true);
            
            Path poiPairbtsCh1MobDataPath = new Path(tmpDirPath,
                                                     "poi_pairbts_ch1_mob_data");
            {
                ConvertTwoIntToMobDataJob job = new ConvertTwoIntToMobDataJob(
                        conf);
                job.configure(poiPairbtsCh1Path, poiPairbtsCh1MobDataPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            {
                AdjSwapPoiIdSt2Job job = new AdjSwapPoiIdSt2Job(conf);
                job.configure(new Path[] { poiPairbtsCh1MobDataPath,
                                           poiPairbtsIndexMobDataPath },
                              poiPairbtsAdjPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            Path nindSpreadPath = new Path(tmpDirPath, "nind_spread");
            {
                AdjSpreadCountJob job = new AdjSpreadCountJob(conf);
                job.configure(poiPairbtsAdjPath, nindSpreadPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            Path numIndexPath = new Path(tmpDirPath, "num_index");
            {
                AdjCountIndexesJob job = new AdjCountIndexesJob(conf);
                job.configure(nindSpreadPath, numIndexPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            numIndicesLeft = conf.getLong("num_indices_left", -1);
            if (numIndicesLeft == -1) {
                throw new IllegalStateException();
            }
        } while (numIndicesLeft > 0);
        
        Path poiPoimodPath = new Path(tmpDirPath, "poi_poimod");
        {
            AdjSpreadTableByPoiIdJob job = new AdjSpreadTableByPoiIdJob(conf);
            job.configure(poisTablePath, poiPoimodPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poiIdPoiPath = new Path(tmpDirPath, "poiId_poi");
        {
            AdjSpreadPoisByPoiIdJob job = new AdjSpreadPoisByPoiIdJob(conf);
            job.configure(poisIdPath, poiIdPoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poiPoimodMobDataPath = new Path(tmpDirPath, "poi_poimod_mob_data");
        {
            ConvertLongToMobDataJob job = new ConvertLongToMobDataJob(conf);
            job.configure(poiPoimodPath, poiPoimodMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path poiIdPoiMobDataPath = new Path(tmpDirPath, "poiId_poi_mob_data");
        {
            ConvertPoiNewToMobDataJob job = new ConvertPoiNewToMobDataJob(conf);
            job.configure(poiIdPoiPath, poiIdPoiMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestModPath = new Path(tmpDirPath,
                                                "points_of_interest_mod");
        {
            AdjJoinNewPoiIdJob job = new AdjJoinNewPoiIdJob(conf);
            job.configure(new Path[] { poiPoimodMobDataPath,
                                       poiIdPoiMobDataPath },
                          pointsOfInterestModPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
      
        Path pointsOfInterestMobDataPath = new Path(tmpDirPath,
                "points_of_interest_mob_data");
        {
            ConvertPoiToMobDataByTwoIntJob job =
                    new ConvertPoiToMobDataByTwoIntJob(conf);
            job.configure(pointsOfInterestPath, pointsOfInterestMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestModMobDataPath = new Path(tmpDirPath,
                "points_of_interest_mod_mob_data");
        {
            ConvertPoiNewToMobDataByTwoIntJob job =
                    new ConvertPoiNewToMobDataByTwoIntJob(conf);
            job.configure(pointsOfInterestModPath,
                          pointsOfInterestModMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            AdjChangePoisIdJob job = new AdjChangePoisIdJob(conf);
            job.configure(new Path[] { pointsOfInterestMobDataPath,
                                       pointsOfInterestModMobDataPath },
                          pointsOfInterestIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
