package es.tid.cosmos.mobility.labelling.client;

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
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.mobility.util.ExportClusterToTextReducer;

/**
 *
 * @author dmicol
 */
public final class ClientLabellingRunner {
    private ClientLabellingRunner() {
    }

    public static CosmosWorkflow run(Path cdrsMobPath,
            Path clientsInfoFilteredPath, Path centroidsPath,
            Path vectorClientClusterPath, Path tmpDirPath, boolean isDebug,
            Configuration conf) throws IOException, InterruptedException,
                                       ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        FileSystem fs = FileSystem.get(conf);

        Path cdrsFilteredPath = new Path(tmpDirPath, "cdrs_filtered");
        CosmosJob cdrsFilteredJob = CosmosJob.createReduceJob(conf,
                "VectorFiltClients",
                SequenceFileInputFormat.class,
                VectorFiltClientsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(cdrsFilteredJob, new Path[] {
            cdrsMobPath, clientsInfoFilteredPath });
        FileOutputFormat.setOutputPath(cdrsFilteredJob, cdrsFilteredPath);
        cdrsFilteredJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(cdrsFilteredJob);

        Path vectorSpreadNodedayhourPath = new Path(tmpDirPath,
                                                    "clients_info_spread");
        CosmosJob vectorSpreadNodedayhourJob = CosmosJob.createReduceJob(conf,
                "VectorSpreadNodedayhour",
                SequenceFileInputFormat.class,
                VectorSpreadNodedayhourReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorSpreadNodedayhourJob,
                                      cdrsFilteredPath);
        FileOutputFormat.setOutputPath(vectorSpreadNodedayhourJob,
                                       vectorSpreadNodedayhourPath);
        vectorSpreadNodedayhourJob.addDependentWorkflow(cdrsFilteredJob);
        wfList.add(vectorSpreadNodedayhourJob);

        Path vectorGetNcomsNodedayhourPath = new Path(tmpDirPath,
                                                      "cliVec_numcoms");
        CosmosJob vectorGetNcomsNodedayhourJob = CosmosJob.createReduceJob(conf,
                "VectorGetNcomsNodedayhour",
                SequenceFileInputFormat.class,
                VectorGetNcomsNodedayhourReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorGetNcomsNodedayhourJob,
                                      vectorSpreadNodedayhourPath);
        FileOutputFormat.setOutputPath(vectorGetNcomsNodedayhourJob,
                                       vectorGetNcomsNodedayhourPath);
        vectorGetNcomsNodedayhourJob.addDependentWorkflow(vectorSpreadNodedayhourJob);
        wfList.add(vectorGetNcomsNodedayhourJob);

        Path vectorCreateNodeDayhourPath = new Path(tmpDirPath, "cliVec_group");
         CosmosJob vectorCreateNodeDayhourJob = CosmosJob.createReduceJob(conf, "VectorCreateNodeDayhour",
                SequenceFileInputFormat.class,
                VectorCreateNodeDayhourReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorCreateNodeDayhourJob,
                                      vectorGetNcomsNodedayhourPath);
        FileOutputFormat.setOutputPath(vectorCreateNodeDayhourJob,
                                       vectorCreateNodeDayhourPath);
        vectorCreateNodeDayhourJob.addDependentWorkflow(vectorGetNcomsNodedayhourJob);
        wfList.add(vectorCreateNodeDayhourJob);

        Path vectorFuseNodeDaygroupPath = new Path(tmpDirPath, "vector_client");
        CosmosJob vectorFuseNodeDaygroupJob = CosmosJob.createReduceJob(conf,
                "VectorFuseNodeDaygroup",
                SequenceFileInputFormat.class,
                VectorFuseNodeDaygroupReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorFuseNodeDaygroupJob,
                                      vectorCreateNodeDayhourPath);
        FileOutputFormat.setOutputPath(vectorFuseNodeDaygroupJob,
                                       vectorFuseNodeDaygroupPath);
        vectorFuseNodeDaygroupJob.addDependentWorkflow(vectorCreateNodeDayhourJob);
        wfList.add(vectorFuseNodeDaygroupJob);

        Path vectorClientNormPath = new Path(tmpDirPath, "vector_client_norm");
        CosmosJob vectorClientNormJob = CosmosJob.createMapJob(conf,
                "VectorNormalized",
                SequenceFileInputFormat.class,
                VectorNormalizedMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorClientNormJob,
                                      vectorFuseNodeDaygroupPath);
        FileOutputFormat.setOutputPath(vectorClientNormJob,
                                       vectorClientNormPath);
        vectorClientNormJob.setDeleteOutputOnExit(!isDebug);
        vectorClientNormJob.addDependentWorkflow(vectorFuseNodeDaygroupJob);
        wfList.add(vectorClientNormJob);

        CosmosJob vectorClientClusterJob = CosmosJob.createReduceJob(conf,
                "ClusterClientGetMinDistance",
                SequenceFileInputFormat.class,
                ClusterClientGetMinDistanceReducer.class,
                SequenceFileOutputFormat.class);
        vectorClientClusterJob.getConfiguration().set("centroids", centroidsPath.toString());
        FileInputFormat.setInputPaths(vectorClientClusterJob, vectorClientNormPath);
        FileOutputFormat.setOutputPath(vectorClientClusterJob,
                                       vectorClientClusterPath);
        vectorClientClusterJob.addDependentWorkflow(vectorClientNormJob);
        wfList.add(vectorClientClusterJob);

        if (isDebug) {
            Path vectorClientClusterTextPath = new Path(tmpDirPath,
                    "vector_client_cluster_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf,"ExportClusterToText",
                        SequenceFileInputFormat.class,
                        ExportClusterToTextReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, vectorClientClusterPath);
                FileOutputFormat.setOutputPath(job, vectorClientClusterTextPath);
                job.addDependentWorkflow(vectorClientClusterJob);
                wfList.add(job);
            }
        }
        return wfList;
    }
}
