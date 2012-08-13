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

package es.tid.cosmos.mobility.labelling.secondhomes;

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
import es.tid.cosmos.mobility.util.ExportPoiToTextByTwoIntReducer;
import es.tid.cosmos.mobility.util.SetMobDataInputIdByTwoIntReducer;

/**
 *
 * @author dmicol
 */
public final class DetectSecondHomesRunner {
    private DetectSecondHomesRunner() {
    }

    public static CosmosWorkflow run(Path cellsMobPath,
            Path pointsOfInterestTemp4Path, Path viClientFuseAccPath,
            Path pairbtsAdjPath, Path pointsOfInterestPath, Path tmpDirPath,
            boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();

        Path btsMobPath = new Path(tmpDirPath, "bts_mob");
        CosmosJob btsMobJob = CosmosJob.createMapJob(conf, "PoiCellToBts",
                SequenceFileInputFormat.class,
                PoiCellToBtsMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(btsMobJob, cellsMobPath);
        FileOutputFormat.setOutputPath(btsMobJob, btsMobPath);
        btsMobJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(btsMobJob);

        Path sechPoiPosPath = new Path(tmpDirPath, "sech_poi_pos");
        CosmosJob sechPoiPosJob = CosmosJob.createReduceJob(conf,
                "PoiJoinPoisBtscoordToPoiPos",
                SequenceFileInputFormat.class,
                PoiJoinPoisBtscoordToPoiPosReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(sechPoiPosJob, new Path[] {
            pointsOfInterestTemp4Path, btsMobPath });
        FileOutputFormat.setOutputPath(sechPoiPosJob, sechPoiPosPath);
        sechPoiPosJob.setDeleteOutputOnExit(!isDebug);
        sechPoiPosJob.addDependentWorkflow(btsMobJob);
        wfList.add(sechPoiPosJob);

        Path nodbtsPoiPath = new Path(tmpDirPath, "nodbts_poi");
        CosmosJob nodbtsPoiJob = CosmosJob.createMapJob(conf,
                "PoiJoinPoisBtscoordToPoi",
                SequenceFileInputFormat.class,
                PoiJoinPoisBtscoordToPoiMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(nodbtsPoiJob, pointsOfInterestTemp4Path);
        FileOutputFormat.setOutputPath(nodbtsPoiJob, nodbtsPoiPath);
        nodbtsPoiJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(nodbtsPoiJob);

        Path sechPoiInoutPath = new Path(tmpDirPath, "vector_bts");
        CosmosJob sechPoiInoutJob = CosmosJob.createReduceJob(conf,
                "PoiJoinPoisViToPoiPos",
                SequenceFileInputFormat.class,
                PoiJoinPoisViToPoiPosReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(sechPoiInoutJob, new Path[] {
            sechPoiPosPath, viClientFuseAccPath });
        FileOutputFormat.setOutputPath(sechPoiInoutJob, sechPoiInoutPath);
        sechPoiInoutJob.setDeleteOutputOnExit(!isDebug);
        sechPoiInoutJob.addDependentWorkflow(sechPoiPosJob);
        wfList.add(sechPoiInoutJob);

        Path nodbtsInoutPath = new Path(tmpDirPath, "nodbts_inout");
        CosmosJob nodbtsInoutJob = CosmosJob.createReduceJob(conf,
                "PoiJoinPoisViToTwoInt",
                SequenceFileInputFormat.class,
                PoiJoinPoisViToTwoIntReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(nodbtsInoutJob, new Path[] {
            sechPoiPosPath, viClientFuseAccPath });
        FileOutputFormat.setOutputPath(nodbtsInoutJob, nodbtsInoutPath);
        nodbtsInoutJob.setDeleteOutputOnExit(!isDebug);
        nodbtsInoutJob.addDependentWorkflow(sechPoiPosJob);
        wfList.add(nodbtsInoutJob);

        Path sechPotSecHomePath = new Path(tmpDirPath, "sech_pot_sec_home");
        CosmosJob sechPotSecHomeJob = CosmosJob.createReduceJob(conf,
                "GetPairsSechomePois",
                SequenceFileInputFormat.class,
                GetPairsSechomePoisReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(sechPotSecHomeJob, sechPoiInoutPath);
        FileOutputFormat.setOutputPath(sechPotSecHomeJob, sechPotSecHomePath);
        sechPotSecHomeJob.setDeleteOutputOnExit(!isDebug);
        sechPotSecHomeJob.addDependentWorkflow(sechPoiInoutJob);
        wfList.add(sechPotSecHomeJob);

        Path sechPotSecHomeInputIdPath = new Path(tmpDirPath, "sech_pot_sec_home_id");
        CosmosJob sechPotSecHomeInputIdJob = CosmosJob.createReduceJob(conf,
                "GetPairsSechomePois",
                SequenceFileInputFormat.class,
                SetMobDataInputIdByTwoIntReducer.class,
                SequenceFileOutputFormat.class);
        sechPotSecHomeInputIdJob.getConfiguration().setInt("input_id", 0);
        FileInputFormat.setInputPaths(sechPotSecHomeInputIdJob, sechPotSecHomePath);
        FileOutputFormat.setOutputPath(sechPotSecHomeInputIdJob, sechPotSecHomeInputIdPath);
        sechPotSecHomeInputIdJob.addDependentWorkflow(sechPotSecHomeJob);
        wfList.add(sechPotSecHomeInputIdJob);

        Path pairbtsAdjInputIdPath = new Path(tmpDirPath, "pairbts_adj_id");
        CosmosJob pairbtsAdjInputIdJob = CosmosJob.createReduceJob(conf,
                "GetPairsSechomePois",
                SequenceFileInputFormat.class,
                SetMobDataInputIdByTwoIntReducer.class,
                SequenceFileOutputFormat.class);
        pairbtsAdjInputIdJob.getConfiguration().setInt("input_id", 1);
        FileInputFormat.setInputPaths(pairbtsAdjInputIdJob, pairbtsAdjPath);
        FileOutputFormat.setOutputPath(pairbtsAdjInputIdJob, pairbtsAdjInputIdPath);
        wfList.add(pairbtsAdjInputIdJob);

        Path nodbtsSechomePath = new Path(tmpDirPath, "nodbts_sechome");
        CosmosJob nodbtsSechomeJob = CosmosJob.createReduceJob(conf,
                "PoiFilterSechomeAdjacent",
                SequenceFileInputFormat.class,
                PoiFilterSechomeAdjacentReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(nodbtsSechomeJob, new Path[] {
            sechPotSecHomeInputIdPath, pairbtsAdjInputIdPath });
        FileOutputFormat.setOutputPath(nodbtsSechomeJob, nodbtsSechomePath);
        nodbtsSechomeJob.setDeleteOutputOnExit(!isDebug);
        nodbtsSechomeJob.addDependentWorkflow(sechPotSecHomeInputIdJob);
        nodbtsSechomeJob.addDependentWorkflow(pairbtsAdjInputIdJob);
        wfList.add(nodbtsSechomeJob);

        Path nodbtsSechomeUniqPath = new Path(tmpDirPath,
                                              "nodbts_sechome_uniq");
        CosmosJob nodbtsSechomeUniqJob = CosmosJob.createReduceJob(conf,
                "PoiDeleteSechomeDuplicate",
                SequenceFileInputFormat.class,
                PoiDeleteSechomeDuplicateReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(nodbtsSechomeUniqJob, nodbtsSechomePath);
        FileOutputFormat.setOutputPath(nodbtsSechomeUniqJob, nodbtsSechomeUniqPath);
        nodbtsSechomeUniqJob.setDeleteOutputOnExit(!isDebug);
        nodbtsSechomeUniqJob.addDependentWorkflow(nodbtsSechomeJob);
        wfList.add(nodbtsSechomeUniqJob);

        CosmosJob pointsOfInterestJob = CosmosJob.createReduceJob(conf,
                "PoiJoinSechomeResults",
                SequenceFileInputFormat.class,
                PoiJoinSechomeResultsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(pointsOfInterestJob, new Path[] {
            nodbtsPoiPath,
            nodbtsInoutPath,
            nodbtsSechomeUniqPath });
        FileOutputFormat.setOutputPath(pointsOfInterestJob, pointsOfInterestPath);
        pointsOfInterestJob.addDependentWorkflow(nodbtsPoiJob);
        pointsOfInterestJob.addDependentWorkflow(nodbtsInoutJob);
        pointsOfInterestJob.addDependentWorkflow(nodbtsSechomeUniqJob);
        wfList.add(pointsOfInterestJob);

        if (isDebug) {
            Path pointsOfInterestTextPath = new Path(tmpDirPath,
                                                     "points_of_interest_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf,
                        "ExportPoiToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestPath);
                FileOutputFormat.setOutputPath(job, pointsOfInterestTextPath);
                job.addDependentWorkflow(pointsOfInterestJob);
                wfList.add(job);
            }
        }
        return wfList;
    }
}
