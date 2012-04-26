package es.tid.cosmos.mobility.labelling.bts;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.labelling.client.VectorCreateNodeDayhourReducer;
import es.tid.cosmos.mobility.labelling.client.VectorFuseNodeDaygroupReducer;
import es.tid.cosmos.mobility.labelling.client.VectorNormalizedReducer;
import es.tid.cosmos.mobility.util.*;

/**
 *
 * @author dmicol
 */
public final class BtsLabellingRunner {
    private BtsLabellingRunner() {
    }
    
    public static void run(Path btsCommsPath, Path btsComareaPath,
                           Path centroidsPath, Path vectorBtsClusterPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path btsCountsPath = new Path(tmpDirPath, "bts_counts");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorFilterBts",
                    SequenceFileInputFormat.class,
                    VectorFilterBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsCommsPath);
            FileOutputFormat.setOutputPath(job, btsCountsPath);
            job.waitForCompletion(true);
        }

        Path btsSumComsPath = new Path(tmpDirPath, "bts_sum_coms");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorSumComsBts",
                    SequenceFileInputFormat.class,
                    VectorSumComsBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsCountsPath);
            FileOutputFormat.setOutputPath(job, btsSumComsPath);
            job.waitForCompletion(true);
        }

        Path btsDayhourPath = new Path(tmpDirPath, "bts_dayhour");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorCreateNodeDayhour",
                    SequenceFileInputFormat.class,
                    VectorCreateNodeDayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsSumComsPath);
            FileOutputFormat.setOutputPath(job, btsDayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsPath = new Path(tmpDirPath, "vector_bts");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorFuseNodeDaygroup",
                    SequenceFileInputFormat.class,
                    VectorFuseNodeDaygroupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsDayhourPath);
            FileOutputFormat.setOutputPath(job, vectorBtsPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsNormPath = new Path(tmpDirPath, "vector_bts_norm");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorNormalized",
                    SequenceFileInputFormat.class,
                    VectorNormalizedReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorBtsPath);
            FileOutputFormat.setOutputPath(job, vectorBtsNormPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsClusterSinfiltPath = new Path(tmpDirPath,
                "vector_bts_cluster_sinfilt");
        {
            ReduceJob job = ReduceJob.create(conf, "ClusterBtsGetMinDistance",
                    SequenceFileInputFormat.class,
                    ClusterBtsGetMinDistanceReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorBtsNormPath);
            FileOutputFormat.setOutputPath(job, vectorBtsClusterSinfiltPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsClusterSinfiltMobDataPath = new Path(tmpDirPath,
                "vector_bts_cluster_sinfilt_mob_data");
        { 
            ReduceJob job = ReduceJob.create(conf, "ConvertClusterToMobData",
                    SequenceFileInputFormat.class,
                    ConvertClusterToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorBtsClusterSinfiltPath);
            FileOutputFormat.setOutputPath(job,
                                           vectorBtsClusterSinfiltMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path btsComareaMobDataPath = new Path(tmpDirPath,
                                              "bts_comarea_mob_data");
        { 
            ReduceJob job = ReduceJob.create(conf, "ConvertBtsToMobData",
                    SequenceFileInputFormat.class,
                    ConvertBtsToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsComareaPath);
            FileOutputFormat.setOutputPath(job, btsComareaMobDataPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "FilterBtsVector",
                    SequenceFileInputFormat.class,
                    FilterBtsVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorBtsClusterSinfiltMobDataPath, btsComareaMobDataPath });
            FileOutputFormat.setOutputPath(job, vectorBtsClusterPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(vectorBtsClusterSinfiltMobDataPath, true);
        fs.delete(btsComareaMobDataPath, true);

        if (isDebug) {
            Path vectorBtsClusterTextPath = new Path(tmpDirPath,
                                                    "vector_bts_cluster_text");
            {
                ReduceJob job = ReduceJob.create(conf, "ExportClusterToText",
                        SequenceFileInputFormat.class,
                        ExportClusterToTextReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, vectorBtsClusterPath);
                FileOutputFormat.setOutputPath(job, vectorBtsClusterTextPath);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(btsCountsPath, true);
            fs.delete(btsCommsPath, true);
            fs.delete(btsSumComsPath, true);
            fs.delete(btsDayhourPath, true);
            fs.delete(vectorBtsPath, true);
            fs.delete(vectorBtsNormPath, true);
            fs.delete(vectorBtsClusterSinfiltPath, true);
        }
    }
}
