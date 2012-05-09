package es.tid.cosmos.mobility.outpois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;

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
            MapJob job = MapJob.create(conf, "PoiSpreadNodebtsVector",
                    SequenceFileInputFormat.class,
                    PoiSpreadNodebtsVectorMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientbtsPath);
            FileOutputFormat.setOutputPath(job, vectorClientbtsSpreadPath);
            job.waitForCompletion(true);
        }

        Path vectorClientpoiPath = new Path(tmpDirPath, "vector_clientpoi");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoivectorPoi",
                    SequenceFileInputFormat.class,
                    PoiJoinPoivectorPoiReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorClientbtsSpreadPath, pointsOfInterestIdPath });
            FileOutputFormat.setOutputPath(job, vectorClientpoiPath);
            job.waitForCompletion(true);
        }
        
        Path vectorPoiClusterPath = new Path(tmpDirPath, "vector_poi_cluster");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiNormalizePoiVector",
                    SequenceFileInputFormat.class,
                    PoiNormalizePoiVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientpoiPath);
            FileOutputFormat.setOutputPath(job, vectorPoiClusterPath);
            job.waitForCompletion(true);
        }

        Path pointsOfInterestIdTxtPath = new Path(tmpDirPath,
                                                  "pointsOfInterestIdTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorPoisOut",
                    SequenceFileInputFormat.class,
                    VectorPoisOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestIdPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestIdTxtPath);
            job.waitForCompletion(true);
        }

        Path vectorCommClientTxtPath = new Path(tmpDirPath,
                                                "vectorCommClientTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorOneidOut",
                    SequenceFileInputFormat.class,
                    VectorOneidOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorClientClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommClientTxtPath);
            job.waitForCompletion(true);
        }

        Path vectorCommBtsTxtPath = new Path(tmpDirPath, "vectorCommBtsTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorOneidOut",
                    SequenceFileInputFormat.class,
                    VectorOneidOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorBtsClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommBtsTxtPath);
            job.waitForCompletion(true);
        }
        
        Path vectorCommPoiTxtPath = new Path(tmpDirPath, "vectorCommPoiTxt");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorNodbtsOut",
                    SequenceFileInputFormat.class,
                    VectorNodbtsOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorPoiClusterPath);
            FileOutputFormat.setOutputPath(job, vectorCommPoiTxtPath);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            fs.delete(pointsOfInterestIdPath, true);
            fs.delete(vectorClientbtsPath, true);
            fs.delete(vectorClientbtsSpreadPath, true);
            fs.delete(vectorClientpoiPath, true);
        }
    }
}
