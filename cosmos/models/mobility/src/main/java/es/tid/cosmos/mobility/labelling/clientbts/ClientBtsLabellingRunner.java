/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.mobility.labelling.clientbts;

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
import es.tid.cosmos.mobility.labelling.client.VectorNormalizedMapper;
import es.tid.cosmos.mobility.util.ExportClusterToTextByTwoIntReducer;
import es.tid.cosmos.mobility.util.SetMobDataInputIdByTwoIntReducer;

/**
 *
 * @author dmicol
 */
public final class ClientBtsLabellingRunner {
    private ClientBtsLabellingRunner() {
    }

    public static CosmosWorkflow run(Path clientsInfoPath,
            Path clientsRepbtsPath, Path vectorClientbtsPath, Path centroidsPath,
            Path pointsOfInterestTempPath, Path vectorClientbtsClusterPath,
            Path tmpDirPath, boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();

        Path clientsbtsSpreadPath = new Path(tmpDirPath, "clientsbts_spread");
        CosmosJob clientsbtsSpreadJob = CosmosJob.createReduceJob(conf,
                "VectorSpreadNodbts",
                SequenceFileInputFormat.class,
                VectorSpreadNodbtsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsbtsSpreadJob, clientsInfoPath);
        FileOutputFormat.setOutputPath(clientsbtsSpreadJob, clientsbtsSpreadPath);
        clientsbtsSpreadJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(clientsbtsSpreadJob);

        Path clientsbtsSumPath = new Path(tmpDirPath, "clientsbts_sum");
        CosmosJob clientsbtsSumJob = CosmosJob.createReduceJob(conf,
                "VectorSumGroupcomms",
                SequenceFileInputFormat.class,
                VectorSumGroupcommsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsbtsSumJob, clientsbtsSpreadPath);
        FileOutputFormat.setOutputPath(clientsbtsSumJob, clientsbtsSumPath);
        clientsbtsSumJob.setDeleteOutputOnExit(!isDebug);
        clientsbtsSumJob.addDependentWorkflow(clientsbtsSpreadJob);
        wfList.add(clientsbtsSumJob);

        Path clientsbtsSumWithInputIdPath = new Path(tmpDirPath,
                "clientsbts_sum_with_input_id");
        CosmosJob clientsbtsSumWithInputIdJob = CosmosJob.createReduceJob(conf,
                "SetMobDataInputIdByTwoInt",
                SequenceFileInputFormat.class,
                SetMobDataInputIdByTwoIntReducer.class,
                SequenceFileOutputFormat.class);
        clientsbtsSumWithInputIdJob.getConfiguration().setInt("input_id", 0);
        FileInputFormat.setInputPaths(clientsbtsSumWithInputIdJob,
                                      clientsbtsSumPath);
        FileOutputFormat.setOutputPath(clientsbtsSumWithInputIdJob,
                                       clientsbtsSumWithInputIdPath);
        clientsbtsSumWithInputIdJob.setDeleteOutputOnExit(true);
        clientsbtsSumWithInputIdJob.addDependentWorkflow(clientsbtsSumJob);
        wfList.add(clientsbtsSumWithInputIdJob);

        Path clientsRepbtsWithInputIdPath = new Path(tmpDirPath,
                "clients_repbts_with_input_id");
        CosmosJob clientsRepbtsWithInputIdJob = CosmosJob.createReduceJob(conf,
                "SetMobDataInputIdByTwoInt",
                SequenceFileInputFormat.class,
                SetMobDataInputIdByTwoIntReducer.class,
                SequenceFileOutputFormat.class);
        clientsRepbtsWithInputIdJob.getConfiguration().setInt("input_id", 1);
        FileInputFormat.setInputPaths(clientsRepbtsWithInputIdJob,
                                      clientsRepbtsPath);
        FileOutputFormat.setOutputPath(clientsRepbtsWithInputIdJob,
                                       clientsRepbtsWithInputIdPath);
        clientsRepbtsWithInputIdJob.setDeleteOutputOnExit(true);
        wfList.add(clientsRepbtsWithInputIdJob);

        Path clientsbtsRepbtsPath = new Path(tmpDirPath, "clientsbts_repbts");
        CosmosJob clientsbtsRepbtsJob = CosmosJob.createReduceJob(conf,
                "VectorFiltClientbts",
                SequenceFileInputFormat.class,
                VectorFiltClientbtsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsbtsRepbtsJob, new Path[] {
            clientsbtsSumWithInputIdPath, clientsRepbtsWithInputIdPath });
        FileOutputFormat.setOutputPath(clientsbtsRepbtsJob, clientsbtsRepbtsPath);
        clientsbtsRepbtsJob.setDeleteOutputOnExit(!isDebug);
        clientsbtsRepbtsJob.addDependentWorkflow(clientsbtsSumWithInputIdJob);
        clientsbtsRepbtsJob.addDependentWorkflow(clientsRepbtsWithInputIdJob);
        wfList.add(clientsbtsRepbtsJob);

        Path clientsbtsGroupPath = new Path(tmpDirPath, "clientsbts_group");
        CosmosJob clientsbtsGroupJob = CosmosJob.createReduceJob(conf,
                "VectorCreateNodeDayhour",
                SequenceFileInputFormat.class,
                VectorCreateNodeDayhourReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsbtsGroupJob, clientsbtsRepbtsPath);
        FileOutputFormat.setOutputPath(clientsbtsGroupJob, clientsbtsGroupPath);
        clientsbtsGroupJob.setDeleteOutputOnExit(!isDebug);
        clientsbtsGroupJob.addDependentWorkflow(clientsbtsRepbtsJob);
        wfList.add(clientsbtsGroupJob);

        CosmosJob vectorClientbtsJob = CosmosJob.createReduceJob(conf,
                "VectorFuseNodeDaygroup",
                SequenceFileInputFormat.class,
                VectorFuseNodeDaygroupReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorClientbtsJob, clientsbtsGroupPath);
        FileOutputFormat.setOutputPath(vectorClientbtsJob, vectorClientbtsPath);
        vectorClientbtsJob.addDependentWorkflow(clientsbtsGroupJob);
        wfList.add(vectorClientbtsJob);

        Path vectorClientbtsNormPath = new Path(tmpDirPath,
                                                "vector_clientbts_norm");
        CosmosJob vectorClientbtsNormJob = CosmosJob.createMapJob(conf,
                "VectorNormalized",
                SequenceFileInputFormat.class,
                VectorNormalizedMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(vectorClientbtsNormJob,
                                      vectorClientbtsPath);
        FileOutputFormat.setOutputPath(vectorClientbtsNormJob,
                                       vectorClientbtsNormPath);
        vectorClientbtsNormJob.setDeleteOutputOnExit(!isDebug);
        vectorClientbtsNormJob.addDependentWorkflow(vectorClientbtsJob);
        wfList.add(vectorClientbtsNormJob);

        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterClientBtsGetMinDistanceToPoi",
                    SequenceFileInputFormat.class,
                    ClusterClientBtsGetMinDistanceToPoiReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorClientbtsNormPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestTempPath);
            job.setDeleteOutputOnExit(!isDebug);
            job.addDependentWorkflow(vectorClientbtsNormJob);
            wfList.add(job);
        }

        CosmosJob vectorClientbtsClusterJob = CosmosJob.createReduceJob(conf,
                "ClusterClientBtsGetMinDistanceToCluster",
                SequenceFileInputFormat.class,
                ClusterClientBtsGetMinDistanceToClusterReducer.class,
                SequenceFileOutputFormat.class);
        vectorClientbtsClusterJob.getConfiguration().set("centroids",
                centroidsPath.toString());
        FileInputFormat.setInputPaths(vectorClientbtsClusterJob,
                                      vectorClientbtsNormPath);
        FileOutputFormat.setOutputPath(vectorClientbtsClusterJob,
                                       vectorClientbtsClusterPath);
        vectorClientbtsClusterJob.setDeleteOutputOnExit(!isDebug);
        vectorClientbtsClusterJob.addDependentWorkflow(vectorClientbtsNormJob);
        wfList.add(vectorClientbtsClusterJob);

        if (isDebug) {
            Path vectorClientbtsClusterTextPath = new Path(tmpDirPath,
                    "vector_clientbts_cluster_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf,
                        "ExportClusterToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportClusterToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, vectorClientbtsClusterPath);
                FileOutputFormat.setOutputPath(job,
                                               vectorClientbtsClusterTextPath);
                job.addDependentWorkflow(vectorClientbtsClusterJob);
                wfList.add(job);
            }
        }
        return wfList;
    }
}
