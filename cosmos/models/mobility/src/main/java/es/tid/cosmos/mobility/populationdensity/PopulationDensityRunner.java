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

package es.tid.cosmos.mobility.populationdensity;

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
import es.tid.cosmos.mobility.activitydensity.PopdenCreateVectorReducer;
import es.tid.cosmos.mobility.activitydensity.PopdenProfileGetOutReducer;
import es.tid.cosmos.mobility.activitydensity.PopdenSumCommsReducer;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenDeleteDuplicatesReducer;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenSpreadNodebtsdayhourReducer;

/**
 *
 * @author ximo
 */
public final class PopulationDensityRunner {
    private PopulationDensityRunner() {
    }

    public static CosmosWorkflow run(Path cdrsInfoPath, Path cellsPath,
                                     Path populationDensityOut, Path tmpDirPath,
                                     boolean isDebug, Configuration conf)
            throws ClassNotFoundException, IOException, InterruptedException {
        WorkflowList wfList = new WorkflowList();
        Path denpobNodbtsdayhourPath = new Path(tmpDirPath,
                                                "denpob_nodbtsdayhour");
        CosmosJob denpobNodbtsdayhourJob = CosmosJob.createReduceJob(conf,
                "PopdenSpreadNodebtsdayhour",
                SequenceFileInputFormat.class,
                PopdenSpreadNodebtsdayhourReducer.class,
                SequenceFileOutputFormat.class);
        denpobNodbtsdayhourJob.getConfiguration().set("cells",
                cellsPath.toString());
        FileInputFormat.setInputPaths(denpobNodbtsdayhourJob, cdrsInfoPath);
        FileOutputFormat.setOutputPath(denpobNodbtsdayhourJob,
                                       denpobNodbtsdayhourPath);
        denpobNodbtsdayhourJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(denpobNodbtsdayhourJob);

        Path denpobNodeinfoNodupPath = new Path(tmpDirPath,
                                                "denpob_nodeinfo_nodup");
        CosmosJob denpobNodeinfoNodupJob = CosmosJob.createReduceJob(conf,
                "PopdenDeleteDuplicates",
                SequenceFileInputFormat.class,
                PopdenDeleteDuplicatesReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(denpobNodeinfoNodupJob,
                                      denpobNodbtsdayhourPath);
        FileOutputFormat.setOutputPath(denpobNodeinfoNodupJob,
                                       denpobNodeinfoNodupPath);
        denpobNodeinfoNodupJob.setDeleteOutputOnExit(!isDebug);
        denpobNodeinfoNodupJob.addDependentWorkflow(denpobNodbtsdayhourJob);
        wfList.add(denpobNodeinfoNodupJob);

        Path popdenprofBtsprofPath = new Path(tmpDirPath,
                                              "popdenprof_btsprof");
        CosmosJob popdenprofBtsprofJob = CosmosJob.createReduceJob(conf,
                "PopdenJoinNodeInfoWithoutProfile",
                SequenceFileInputFormat.class,
                PopdenJoinNodeInfoWithoutProfileReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(popdenprofBtsprofJob,
                                      denpobNodeinfoNodupPath);
        FileOutputFormat.setOutputPath(popdenprofBtsprofJob,
                                       popdenprofBtsprofPath);
        popdenprofBtsprofJob.addDependentWorkflow(denpobNodeinfoNodupJob);
        wfList.add(popdenprofBtsprofJob);

        Path popdenBtsprofCountPath = new Path(tmpDirPath,
                                               "popden_btsprof_count");
        CosmosJob popdenBtsprofCountJob = CosmosJob.createReduceJob(conf,
                "PopdenSumComms",
                SequenceFileInputFormat.class,
                PopdenSumCommsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(popdenBtsprofCountJob,
                                      popdenprofBtsprofPath);
        FileOutputFormat.setOutputPath(popdenBtsprofCountJob,
                                       popdenBtsprofCountPath);
        popdenBtsprofCountJob.setDeleteOutputOnExit(!isDebug);
        popdenBtsprofCountJob.addDependentWorkflow(popdenprofBtsprofJob);
        wfList.add(popdenBtsprofCountJob);

        Path populationDensityPath = new Path(tmpDirPath,
                                              "population_density");
        CosmosJob populationDensityJob = CosmosJob.createReduceJob(conf,
                "PopdenSumComms",
                SequenceFileInputFormat.class,
                PopdenCreateVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(populationDensityJob,
                                      popdenBtsprofCountPath);
        FileOutputFormat.setOutputPath(populationDensityJob,
                                       populationDensityPath);
        populationDensityJob.setDeleteOutputOnExit(!isDebug);
        populationDensityJob.addDependentWorkflow(popdenBtsprofCountJob);
        wfList.add(populationDensityJob);

        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenProfileGetOut",
                    SequenceFileInputFormat.class,
                    PopdenProfileGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, populationDensityPath);
            FileOutputFormat.setOutputPath(job, populationDensityOut);
            job.addDependentWorkflow(populationDensityJob);
            wfList.add(job);
        }

        return wfList;
    }
}
