package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.mobility.util.ExportPoiToTextReducer;

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
            throws IOException, InterruptedException, ClassNotFoundException {
        FileSystem fs = FileSystem.get(conf);
        
        Path pointsOfInterestTemp2Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp2");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterAggNodeClusterByNodbts",
                    SequenceFileInputFormat.class,
                    ClusterAggNodeClusterByNodbtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTempPath, vectorClientClusterPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestTemp2Path);
            job.waitForCompletion(true);
        }

        Path potpoiPath = new Path(tmpDirPath, "potpoi");
        { 
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterAggNodeClusterByNodlbl",
                    SequenceFileInputFormat.class,
                    ClusterAggNodeClusterByNodlblReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTempPath, vectorClientClusterPath });
            FileOutputFormat.setOutputPath(job, potpoiPath);
            job.waitForCompletion(true);
        }
        
        Path clientbtsNodpoilblPath = new Path(tmpDirPath,
                                               "clientbts_nodpoilbl");
        { 
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterSpreadNodelblPoilbl",
                    SequenceFileInputFormat.class,
                    ClusterSpreadNodelblPoilblReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestTemp2Path);
            FileOutputFormat.setOutputPath(job, clientbtsNodpoilblPath);
            job.waitForCompletion(true);
        }
        
        Path clientbtsNodpoiCountPath = new Path(tmpDirPath,
                                                 "clientbts_nodpoi_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ClusterCountMajPoiByNode",
                    SequenceFileInputFormat.class,
                    ClusterCountMajPoiByNodeReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientbtsNodpoilblPath);
            FileOutputFormat.setOutputPath(job, clientbtsNodpoiCountPath);
            job.waitForCompletion(true);
        }
        
        Path clientbtsNodPoimajPath = new Path(tmpDirPath,
                                               "clientbts_nod_poimaj");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ClusterGetMajPoiByNode",
                    SequenceFileInputFormat.class,
                    ClusterGetMajPoiByNodeReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientbtsNodpoiCountPath);
            FileOutputFormat.setOutputPath(job, clientbtsNodPoimajPath);
            job.waitForCompletion(true);
        }

        Path poisLabeledPath = new Path(tmpDirPath, "pois_labeled");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ClusterJoinPotPoiLabel",
                    SequenceFileInputFormat.class,
                    ClusterJoinPotPoiLabelReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                potpoiPath, clientbtsNodPoimajPath });
            FileOutputFormat.setOutputPath(job, poisLabeledPath);
            job.waitForCompletion(true);
        }
        
        Path pointsOfInterestTemp3Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp3");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterAggPotPoiPoisToPoi",
                    SequenceFileInputFormat.class,
                    ClusterAggPotPoiPoisToPoiReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTemp2Path, poisLabeledPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestTemp3Path);
            job.waitForCompletion(true);
        }
        
        Path vectorClientbtsClusterAddPath = new Path(tmpDirPath,
                "vector_clientbts_cluster_add");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterAggPotPoiPoisToCluster",
                    SequenceFileInputFormat.class,
                    ClusterAggPotPoiPoisToClusterReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorClientbtsClusterPath, poisLabeledPath });
            FileOutputFormat.setOutputPath(job, vectorClientbtsClusterAddPath);
            job.waitForCompletion(true);
        }
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ClusterAggBtsCluster",
                    SequenceFileInputFormat.class,
                    ClusterAggBtsClusterReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTemp3Path, vectorBtsClusterPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestTemp4Path);
            job.waitForCompletion(true);
        }
        
        if (isDebug) {
            Path pointsOfInterestTemp4TextPath = new Path(tmpDirPath,
                    "points_of_interest_temp4_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf, "ExportPoiToText",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
                FileOutputFormat.setOutputPath(job,
                                               pointsOfInterestTemp4TextPath);
                job.waitForCompletion(true);
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
