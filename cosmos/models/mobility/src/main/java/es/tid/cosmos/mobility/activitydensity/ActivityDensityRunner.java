package es.tid.cosmos.mobility.activitydensity;

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

/**
 *
 * @author dmicol
 */
public final class ActivityDensityRunner {
    private ActivityDensityRunner() {
    }

    public static CosmosWorkflow run(Path clientProfileMobPath,
            Path populationDensityOutPath, Path tmpDirPath, boolean isDebug,
            Configuration conf)
            throws ClassNotFoundException, IOException, InterruptedException {
        WorkflowList wfList = new WorkflowList();
        Path popdenBtsPath = new Path(tmpDirPath, "popden_bts");
        CosmosJob popdenBtsJob = CosmosJob.createReduceJob(conf, "PopdenSpreadArray",
                SequenceFileInputFormat.class,
                PopdenSpreadArrayReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(popdenBtsJob, clientProfileMobPath);
        FileOutputFormat.setOutputPath(popdenBtsJob, popdenBtsPath);
        popdenBtsJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(popdenBtsJob);

        Path popdenBtsCountPath = new Path(tmpDirPath, "popden_bts_count");
        CosmosJob popdenBtsCountJob = CosmosJob.createReduceJob(conf,
                "PopdenSumComms",
                SequenceFileInputFormat.class,
                PopdenSumCommsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(popdenBtsCountJob, popdenBtsPath);
        FileOutputFormat.setOutputPath(popdenBtsCountJob, popdenBtsCountPath);
        popdenBtsCountJob.addDependentWorkflow(popdenBtsJob);
        wfList.add(popdenBtsCountJob);

        Path populationDensityPath = new Path(tmpDirPath, "population_density");
        CosmosJob populationDensityJob = CosmosJob.createReduceJob(conf,
                "PopdenCreateVector",
                SequenceFileInputFormat.class,
                PopdenCreateVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(populationDensityJob, popdenBtsCountPath);
        FileOutputFormat.setOutputPath(populationDensityJob, populationDensityPath);
        populationDensityJob.setDeleteOutputOnExit(!isDebug);
        populationDensityJob.addDependentWorkflow(popdenBtsCountJob);
        wfList.add(populationDensityJob);

        CosmosJob job = CosmosJob.createReduceJob(conf,
                "PopdenProfileGetOut",
                SequenceFileInputFormat.class,
                PopdenProfileGetOutReducer.class,
                1,
                TextOutputFormat.class);
        FileInputFormat.setInputPaths(job, populationDensityPath);
        FileOutputFormat.setOutputPath(job, populationDensityOutPath);
        job.addDependentWorkflow(populationDensityJob);
        wfList.add(job);

        return wfList;
    }
}
