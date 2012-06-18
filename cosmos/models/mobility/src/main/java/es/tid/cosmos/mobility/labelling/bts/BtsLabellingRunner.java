package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.mobility.labelling.client.VectorCreateNodeDayhourReducer;
import es.tid.cosmos.mobility.labelling.client.VectorFuseNodeDaygroupReducer;
import es.tid.cosmos.mobility.labelling.client.VectorNormalizedReducer;
import es.tid.cosmos.mobility.util.ExportClusterToTextReducer;

/**
 *
 * @author dmicol
 */
public class BtsLabellingRunner {
    public BtsLabellingRunner() {
    }
    
    public CosmosWorkflow run(Path btsCommsPath, Path btsComareaPath,
            Path centroidsPath, Path vectorBtsClusterPath, Path tmpDirPath,
            boolean isDebug, Configuration conf) throws IOException,
                                                        InterruptedException,
                                                        ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        
        Path btsCountsPath = new Path(tmpDirPath, "bts_counts");
        CosmosJob btsCountsJob = CosmosJob.createMapJob(conf, "VectorFilterBts",
                SequenceFileInputFormat.class,
                VectorFilterBtsMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(btsCountsJob, btsCommsPath);
        FileOutputFormat.setOutputPath(btsCountsJob, btsCountsPath);
        btsCountsJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(btsCountsJob);

        Path btsSumComsPath = new Path(tmpDirPath, "bts_sum_coms");
        CosmosJob btsSumComsJob = CosmosJob.createReduceJob(conf,
                "VectorSumComsBts",
                SequenceFileInputFormat.class,
                VectorSumComsBtsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(btsSumComsJob, btsCountsPath);
        FileOutputFormat.setOutputPath(btsSumComsJob, btsSumComsPath);
        btsSumComsJob.setDeleteOutputOnExit(!isDebug);
        btsSumComsJob.addDependentWorkflow(btsCountsJob);
        wfList.add(btsSumComsJob);

        Path btsDayhourPath = new Path(tmpDirPath, "bts_dayhour");
        CosmosJob btsDayhourJob = CosmosJob.createReduceJob(conf,
                "VectorCreateNodeDayhour",
                SequenceFileInputFormat.class,
                VectorCreateNodeDayhourReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(btsDayhourJob, btsSumComsPath);
        FileOutputFormat.setOutputPath(btsDayhourJob, btsDayhourPath);
        btsDayhourJob.setDeleteOutputOnExit(!isDebug);
        btsDayhourJob.addDependentWorkflow(btsSumComsJob);
        wfList.add(btsDayhourJob);

        Path vectorBtsPath = new Path(tmpDirPath, "vector_bts");
        CosmosJob vectorBtsJob = CosmosJob.createReduceJob(conf,
                "VectorFuseNodeDaygroup",
                SequenceFileInputFormat.class,
                VectorFuseNodeDaygroupReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorBtsJob, btsDayhourPath);
        FileOutputFormat.setOutputPath(vectorBtsJob, vectorBtsPath);
        vectorBtsJob.setDeleteOutputOnExit(!isDebug);
        vectorBtsJob.addDependentWorkflow(btsDayhourJob);
        wfList.add(vectorBtsJob);

        Path vectorBtsNormPath = new Path(tmpDirPath, "vector_bts_norm");
        CosmosJob vectorBtsNormJob = CosmosJob.createReduceJob(conf,
                "VectorNormalized",
                SequenceFileInputFormat.class,
                VectorNormalizedReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorBtsNormJob, vectorBtsPath);
        FileOutputFormat.setOutputPath(vectorBtsNormJob, vectorBtsNormPath);
        vectorBtsNormJob.setDeleteOutputOnExit(!isDebug);
        vectorBtsNormJob.addDependentWorkflow(vectorBtsJob);
        wfList.add(vectorBtsNormJob);

        Path vectorBtsClusterSinfiltPath = new Path(tmpDirPath,
                "vector_bts_cluster_sinfilt");
        CosmosJob vectorBtsClusterSinfiltJob = CosmosJob.createReduceJob(conf,
                "ClusterBtsGetMinDistance",
                SequenceFileInputFormat.class,
                ClusterBtsGetMinDistanceReducer.class,
                SequenceFileOutputFormat.class);
        vectorBtsClusterSinfiltJob.getConfiguration().set("centroids",
                centroidsPath.toString());
        FileInputFormat.setInputPaths(vectorBtsClusterSinfiltJob,
                                      vectorBtsNormPath);
        FileOutputFormat.setOutputPath(vectorBtsClusterSinfiltJob,
                                       vectorBtsClusterSinfiltPath);
        vectorBtsClusterSinfiltJob.setDeleteOutputOnExit(!isDebug);
        vectorBtsClusterSinfiltJob.addDependentWorkflow(vectorBtsNormJob);
        wfList.add(vectorBtsClusterSinfiltJob);

        CosmosJob vectorBtsClusterJob = CosmosJob.createReduceJob(conf,
                "FilterBtsVector",
                SequenceFileInputFormat.class,
                FilterBtsVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorBtsClusterJob, new Path[] {
            vectorBtsClusterSinfiltPath, btsComareaPath });
        FileOutputFormat.setOutputPath(vectorBtsClusterJob, vectorBtsClusterPath);
        vectorBtsClusterJob.addDependentWorkflow(vectorBtsClusterSinfiltJob);
        wfList.add(vectorBtsClusterJob);

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
                job.addDependentWorkflow(vectorBtsClusterJob);
                wfList.add(job);
            }
        }
        
        return wfList;
    }
}
