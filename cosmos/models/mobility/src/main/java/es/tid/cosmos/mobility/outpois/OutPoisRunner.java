package es.tid.cosmos.mobility.outpois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.ConvertClusterVectorToMobDataByTwoIntJob;
import es.tid.cosmos.mobility.util.ConvertPoiToMobDataByTwoIntJob;

/**
 *
 * @author dmicol
 */
public final class OutPoisRunner {
    private OutPoisRunner() {
    }
    
    public static void run(Path vectorClientbtsPath, Path pointsOfInterestIdPath,
                           Path vectorClientClusterPath,
                           Path vectorBtsClusterPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path vectorClientbtsSpreadPath = new Path(tmpDirPath,
                                                  "vector_clientbts_spread");
        {
            PoiSpreadNodebtsVectorJob job = new PoiSpreadNodebtsVectorJob(conf);
            job.configure(vectorClientbtsPath, vectorClientbtsSpreadPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path vectorClientbtsSpreadMobDataPath = new Path(tmpDirPath,
                "vector_clientbts_spread_mob_data");
        {
            ConvertClusterVectorToMobDataByTwoIntJob job =
                    new ConvertClusterVectorToMobDataByTwoIntJob(conf);
            job.configure(vectorClientbtsSpreadPath,
                          vectorClientbtsSpreadMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path pointsOfInterestIdMobDataPath = new Path(tmpDirPath,
                "points_of_interest_id_mob_data");
        {
            ConvertPoiToMobDataByTwoIntJob job =
                    new ConvertPoiToMobDataByTwoIntJob(conf);
            job.configure(pointsOfInterestIdPath, pointsOfInterestIdMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientpoiPath = new Path(tmpDirPath, "vector_clientpoi");
        {
            PoiJoinPoivectorPoiJob job = new PoiJoinPoivectorPoiJob(conf);
            job.configure(new Path[] { vectorClientbtsSpreadMobDataPath,
                                       pointsOfInterestIdMobDataPath },
                          vectorClientpoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(vectorClientbtsSpreadMobDataPath, true);
        fs.delete(pointsOfInterestIdMobDataPath, true);
        
        Path vectorPoiClusterPath = new Path(tmpDirPath, "vector_poi_cluster");
        {
            PoiNormalizePoiVectorJob job = new PoiNormalizePoiVectorJob(conf);
            job.configure(vectorClientpoiPath, vectorPoiClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path pointsOfInterestIdTxtPath = new Path(tmpDirPath,
                                                  "pointsOfInterestIdTxt");
        {
            VectorPoisOutJob job = new VectorPoisOutJob(conf);
            job.configure(pointsOfInterestIdPath, pointsOfInterestIdTxtPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorCommClientTxtPath = new Path(tmpDirPath,
                                                "vectorCommClientTxt");
        {
            VectorOneidOutJob job = new VectorOneidOutJob(conf);
            job.configure(vectorClientClusterPath, vectorCommClientTxtPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorCommBtsTxtPath = new Path(tmpDirPath, "vectorCommBtsTxt");
        {
            VectorOneidOutJob job = new VectorOneidOutJob(conf);
            job.configure(vectorBtsClusterPath, vectorCommBtsTxtPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path vectorCommPoiTxtPath = new Path(tmpDirPath, "vectorCommPoiTxt");
        {
            VectorNodbtsOutJob job = new VectorNodbtsOutJob(conf);
            job.configure(vectorPoiClusterPath, vectorCommPoiTxtPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        if (!isDebug) {
            fs.delete(pointsOfInterestIdPath, true);
            fs.delete(vectorClientbtsPath, true);
            fs.delete(vectorClientbtsSpreadPath, true);
            fs.delete(vectorClientpoiPath, true);
        }
    }
}
