package es.tid.cosmos.mobility.adjacentextraction;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class AdjacentExtractionRunner {
    private AdjacentExtractionRunner() {
    }
    
    public static void run(Path pointsOfInterestPath,
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

        Path poiPairbtsAdjPath = new Path(tmpDirPath, "poi_pairbts_adj");
        {
            AdjJoinPairbtsAdjbtsJob job = new AdjJoinPairbtsAdjbtsJob(conf);
            job.configure(new Path[] { poiPairbtsPath, poisTablePath },
                          poiPairbtsAdjPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        long numIndicesLeft = 1;
        while (numIndicesLeft > 0) {
            Path poiPairbtsIndexPath = new Path(tmpDirPath,
                                                "poi_pairbts_index");
            {
                AdjPutMaxIdJob job = new AdjPutMaxIdJob(conf);
                job.configure(poiPairbtsAdjPath, poiPairbtsIndexPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            Path poisTableTmpPath = new Path(tmpDirPath, "pois_table_tmp");
            {
                AdjUpdatePoisTableJob job = new AdjUpdatePoisTableJob(conf);
                job.configure(new Path[] { poisTablePath, poiPairbtsIndexPath },
                              poisTableTmpPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            fs.delete(poisTablePath, true);
            fs.rename(poisTableTmpPath, poisTablePath);
            
            Path poiPairbtsCh1Path = new Path(tmpDirPath, "poi_pairbts_ch1");
            {
                AdjSwapPoiIdSt1Job job = new AdjSwapPoiIdSt1Job(conf);
                job.configure(new Path[] { poiPairbtsAdjPath,
                                           poiPairbtsIndexPath },
                              poiPairbtsCh1Path);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            fs.delete(poiPairbtsAdjPath, true);
            
            {
                AdjSwapPoiIdSt1Job job = new AdjSwapPoiIdSt1Job(conf);
                job.configure(new Path[] { poiPairbtsCh1Path,
                                           poiPairbtsIndexPath },
                              poiPairbtsAdjPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
            
            fs.delete(poiPairbtsAdjPath, true);
            
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
        }
        
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
        
        Path pointsOfInterestModPath = new Path(tmpDirPath,
                                                "points_of_interest_mod");
        {
            AdjJoinNewPoiIdJob job = new AdjJoinNewPoiIdJob(conf);
            job.configure(new Path[] { poiPoimodPath, poiIdPoiPath },
                          pointsOfInterestModPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            AdjChangePoisIdJob job = new AdjChangePoisIdJob(conf);
            job.configure(new Path[] { pointsOfInterestPath,
                                       pointsOfInterestModPath },
                          pointsOfInterestIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
