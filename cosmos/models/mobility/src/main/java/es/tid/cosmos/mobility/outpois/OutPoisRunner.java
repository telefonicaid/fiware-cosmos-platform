package es.tid.cosmos.mobility.outpois;

import java.util.EnumSet;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CleanupOptions;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.util.*;

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
            ReduceJob job = ReduceJob.create(conf, "PoiSpreadNodebtsVector",
                    SequenceFileInputFormat.class,
                    PoiSpreadNodebtsVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientbtsPath);
            FileOutputFormat.setOutputPath(job, vectorClientbtsSpreadPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path vectorClientbtsSpreadMobDataPath = new Path(tmpDirPath,
                "vector_clientbts_spread_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertClusterVectorToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertClusterVectorToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientbtsSpreadPath);
            FileOutputFormat.setOutputPath(job,
                                           vectorClientbtsSpreadMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path pointsOfInterestIdMobDataPath = new Path(tmpDirPath,
                "points_of_interest_id_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertPoiToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertPoiToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestIdPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestIdMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path vectorClientpoiPath = new Path(tmpDirPath, "vector_clientpoi");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoivectorPoi",
                    SequenceFileInputFormat.class,
                    PoiJoinPoivectorPoiReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorClientbtsSpreadMobDataPath,
                pointsOfInterestIdMobDataPath });
            FileOutputFormat.setOutputPath(job, vectorClientpoiPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        fs.delete(vectorClientbtsSpreadMobDataPath, true);
        fs.delete(pointsOfInterestIdMobDataPath, true);
        
        Path vectorPoiClusterPath = new Path(tmpDirPath, "vector_poi_cluster");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiNormalizePoiVector",
                    SequenceFileInputFormat.class,
                    PoiNormalizePoiVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientpoiPath);
            FileOutputFormat.setOutputPath(job, vectorPoiClusterPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path pointsOfInterestIdTxtPath = new Path(tmpDirPath,
                                                  "pointsOfInterestIdTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorPoisOut",
                    SequenceFileInputFormat.class,
                    VectorPoisOutReducer.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestIdPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestIdTxtPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path vectorCommClientTxtPath = new Path(tmpDirPath,
                                                "vectorCommClientTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorOneidOut",
                    SequenceFileInputFormat.class,
                    VectorOneidOutReducer.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommClientTxtPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path vectorCommBtsTxtPath = new Path(tmpDirPath, "vectorCommBtsTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorOneidOut",
                    SequenceFileInputFormat.class,
                    VectorOneidOutReducer.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorBtsClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommBtsTxtPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path vectorCommPoiTxtPath = new Path(tmpDirPath, "vectorCommPoiTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorNodbtsOut",
                    SequenceFileInputFormat.class,
                    VectorNodbtsOutReducer.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorPoiClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommPoiTxtPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        if (!isDebug) {
            fs.delete(pointsOfInterestIdPath, true);
            fs.delete(vectorClientbtsPath, true);
            fs.delete(vectorClientbtsSpreadPath, true);
            fs.delete(vectorClientpoiPath, true);
        }
    }
}
