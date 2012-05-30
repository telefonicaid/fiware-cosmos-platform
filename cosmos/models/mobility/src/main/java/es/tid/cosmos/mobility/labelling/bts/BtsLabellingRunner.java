package es.tid.cosmos.mobility.labelling.bts;

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
            throws IOException, InterruptedException, ClassNotFoundException {
        FileSystem fs = FileSystem.get(conf);
        
        Path btsCountsPath = new Path(tmpDirPath, "bts_counts");
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "VectorFilterBts",
                    SequenceFileInputFormat.class,
                    VectorFilterBtsMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsCommsPath);
            FileOutputFormat.setOutputPath(job, btsCountsPath);
            job.waitForCompletion(true);
        }

        Path btsSumComsPath = new Path(tmpDirPath, "bts_sum_coms");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "VectorSumComsBts",
                    SequenceFileInputFormat.class,
                    VectorSumComsBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsCountsPath);
            FileOutputFormat.setOutputPath(job, btsSumComsPath);
            job.waitForCompletion(true);
        }

        Path btsDayhourPath = new Path(tmpDirPath, "bts_dayhour");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "VectorCreateNodeDayhour",
                    SequenceFileInputFormat.class,
                    VectorCreateNodeDayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsSumComsPath);
            FileOutputFormat.setOutputPath(job, btsDayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsPath = new Path(tmpDirPath, "vector_bts");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "VectorFuseNodeDaygroup",
                    SequenceFileInputFormat.class,
                    VectorFuseNodeDaygroupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsDayhourPath);
            FileOutputFormat.setOutputPath(job, vectorBtsPath);
            job.waitForCompletion(true);
        }

        Path vectorBtsNormPath = new Path(tmpDirPath, "vector_bts_norm");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorNormalized",
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
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterBtsGetMinDistance",
                    SequenceFileInputFormat.class,
                    ClusterBtsGetMinDistanceReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorBtsNormPath);
            FileOutputFormat.setOutputPath(job, vectorBtsClusterSinfiltPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "FilterBtsVector",
                    SequenceFileInputFormat.class,
                    FilterBtsVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorBtsClusterSinfiltPath, btsComareaPath });
            FileOutputFormat.setOutputPath(job, vectorBtsClusterPath);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path vectorBtsClusterTextPath = new Path(tmpDirPath,
                                                    "vector_bts_cluster_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf,
                        "ExportClusterToText",
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
