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
    
    public static void run(Path pointsOfInterestPath, Path btsComareaPath,
                           Path centroidsPath, Path vectorBtsClusterPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
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
        
        boolean isFinished = false;
        while (isFinished) {
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
        }
        
        Path poiPoimodPath = new Path(tmpDirPath, "poi_poimod");
        {
            AdjSpreadTableByPoiIdJob job = new AdjSpreadTableByPoiIdJob(conf);
            job.configure(poisTablePath, poiPoimodPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poiIdoiPath = new Path(tmpDirPath, "poiId_poi");
        {
            AdjSpreadPoisByPoiIdJob job = new AdjSpreadPoisByPoiIdJob(conf);
            job.configure(poisIdPath, poiIdoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
