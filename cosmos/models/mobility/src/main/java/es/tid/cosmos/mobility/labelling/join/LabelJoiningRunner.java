package es.tid.cosmos.mobility.labelling.join;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.*;


/**
 *
 * @author dmicol
 */
public final class LabelJoiningRunner {
    private LabelJoiningRunner() {
    }
    
    public static void run(Path pointsOfInterestTempPath,
                           Path vectorClientClusterPath,
                           Path vectorClientbtsClusterPath,
                           Path vectorBtsClusterPath,
                           Path pointsOfInterestTemp4Path,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path pointsOfInterestTempMobDataPath = new Path(tmpDirPath,
                "points_of_interest_temp_mob_data");
        {
            ConvertPoiToMobDataJob job = new ConvertPoiToMobDataJob(conf);
            job.configure(pointsOfInterestTempPath,
                          pointsOfInterestTempMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path vectorClientClusterMobDataPath = new Path(tmpDirPath,
                "vector_client_cluster_mob_data");
        {
            ConvertClusterToMobDataJob job = new ConvertClusterToMobDataJob(
                    conf);
            job.configure(vectorClientClusterPath,
                          vectorClientClusterMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestTemp2Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp2");
        {
            ClusterAggNodeClusterByNodbtsJob job =
                    new ClusterAggNodeClusterByNodbtsJob(conf);
            job.configure(new Path[] { pointsOfInterestTempMobDataPath,
                                       vectorClientClusterMobDataPath },
                          pointsOfInterestTemp2Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path potpoiPath = new Path(tmpDirPath, "potpoi");
        { 
            ClusterAggNodeClusterByNodlblJob job =
                    new ClusterAggNodeClusterByNodlblJob(conf);
            job.configure(new Path[] { pointsOfInterestTempMobDataPath,
                                       vectorClientClusterMobDataPath },
                          potpoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(vectorClientClusterMobDataPath, true);
        fs.delete(pointsOfInterestTempMobDataPath, true);
        
        Path clientbtsNodpoilblPath = new Path(tmpDirPath,
                                               "clientbts_nodpoilbl");
        { 
            ClusterSpreadNodelblPoilblJob job =
                    new ClusterSpreadNodelblPoilblJob(conf);
            job.configure(pointsOfInterestTemp2Path, clientbtsNodpoilblPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientbtsNodpoiCountPath = new Path(tmpDirPath,
                                                 "clientbts_nodpoi_count");
        {
            ClusterCountMajPoiByNodeJob job = new ClusterCountMajPoiByNodeJob(
                    conf);
            job.configure(clientbtsNodpoilblPath, clientbtsNodpoiCountPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientbtsNodPoimajPath = new Path(tmpDirPath,
                                               "clientbts_nod_poimaj");
        {
            ClusterGetMajPoiByNodeJob job = new ClusterGetMajPoiByNodeJob(conf);
            job.configure(clientbtsNodpoiCountPath, clientbtsNodPoimajPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path potpoiMobDataPath = new Path(tmpDirPath, "potpoi_mob_data");
        {
            ConvertPoiToMobDataJob job = new ConvertPoiToMobDataJob(conf);
            job.configure(potpoiPath, potpoiMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientbtsNodPoimajMobDataPath = new Path(tmpDirPath,
                "clientbts_nod_poimaj_mob_data");
        {
            ConvertIntToMobDataJob job = new ConvertIntToMobDataJob(conf);
            job.configure(clientbtsNodPoimajPath, clientbtsNodPoimajMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path poisLabeledPath = new Path(tmpDirPath, "pois_labeled");
        {
            ClusterJoinPotPoiLabelJob job = new ClusterJoinPotPoiLabelJob(conf);
            job.configure(new Path[] { potpoiMobDataPath,
                                       clientbtsNodPoimajMobDataPath },
                          poisLabeledPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        fs.delete(potpoiMobDataPath, true);
        fs.delete(clientbtsNodPoimajMobDataPath, true);
        
        Path pointsOfInterestTemp2MobDataPath = new Path(tmpDirPath,
                "points_of_interest_temp2_mob_data");
        {
            ConvertPoiToMobDataByTwoIntJob job =
                    new ConvertPoiToMobDataByTwoIntJob(conf);
            job.configure(pointsOfInterestTemp2Path,
                          pointsOfInterestTemp2MobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientbtsClusterMobDataPath = new Path(tmpDirPath,
                "vector_clientbts_cluster");
        {
            ConvertClusterToMobDataByTwoIntJob job =
                    new ConvertClusterToMobDataByTwoIntJob(conf);
            job.configure(vectorClientbtsClusterPath,
                          vectorClientbtsClusterMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poisLabeledMobDataPath = new Path(tmpDirPath,
                                               "pois_labeled_mob_data");
        {
            ConvertNullToMobDataByTwoIntJob job =
                    new ConvertNullToMobDataByTwoIntJob(conf);
            job.configure(poisLabeledPath, poisLabeledMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestTemp3Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp3");
        {
            ClusterAggPotPoiPoisToPoiJob job =
                    new ClusterAggPotPoiPoisToPoiJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp2MobDataPath,
                                       vectorClientbtsClusterMobDataPath,
                                       poisLabeledMobDataPath },
                          pointsOfInterestTemp3Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path vectorClientbtsClusterAddPath = new Path(tmpDirPath,
                "vector_clientbts_cluster_add");
        {
            ClusterAggPotPoiPoisToClusterJob job =
                    new ClusterAggPotPoiPoisToClusterJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp2MobDataPath,
                                       vectorClientbtsClusterMobDataPath,
                                       poisLabeledMobDataPath },
                          vectorClientbtsClusterAddPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        fs.delete(pointsOfInterestTemp2MobDataPath, true);
        fs.delete(vectorClientbtsClusterMobDataPath, true);
        fs.delete(poisLabeledMobDataPath, true);
        
        Path pointsOfInterestTemp3MobDataPath = new Path(tmpDirPath,
                "points_of_interest_temp3_mob_data");
        {
            ConvertPoiToMobDataJob job = new ConvertPoiToMobDataJob(conf);
            job.configure(pointsOfInterestTemp3Path,
                          pointsOfInterestTemp3MobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsClusterMobDataPath = new Path(tmpDirPath,
                "vector_bts_cluster_mob_data");
        {
            ConvertClusterToMobDataJob job = new ConvertClusterToMobDataJob(
                    conf);
            job.configure(vectorBtsClusterPath,
                          vectorBtsClusterMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            ClusterAggBtsClusterJob job = new ClusterAggBtsClusterJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp3MobDataPath,
                                       vectorBtsClusterMobDataPath },
                          pointsOfInterestTemp4Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        fs.delete(pointsOfInterestTemp3MobDataPath, true);
        fs.delete(vectorBtsClusterMobDataPath, true);
        
        if (isDebug) {
            Path pointsOfInterestTemp4TextPath = new Path(tmpDirPath,
                    "points_of_interest_temp4_text");
            {
                ExportClusterAggBtsClusterToTextJob job =
                        new ExportClusterAggBtsClusterToTextJob(conf);
                job.configure(pointsOfInterestTemp4Path,
                            pointsOfInterestTemp4TextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        } else {
            fs.delete(potpoiPath, true);
            fs.delete(clientbtsNodpoilblPath, true);
            fs.delete(clientbtsNodpoiCountPath, true);
            fs.delete(clientbtsNodPoimajPath, true);
            fs.delete(poisLabeledPath, true);
            fs.delete(pointsOfInterestTempPath, true);
            fs.delete(pointsOfInterestTemp2Path, true);
            fs.delete(pointsOfInterestTemp3Path, true);
            fs.delete(vectorClientbtsClusterPath, true);
            fs.delete(vectorClientbtsClusterAddPath, true);
        }
    }
}
