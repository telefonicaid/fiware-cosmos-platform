package es.tid.cosmos.mobility.activitydensity.profile;

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

/**
 *
 * @author dmicol
 */
public class ActivityDensityProfileRunner {
    public ActivityDensityProfileRunner() {
    }
    
    public CosmosWorkflow run(Path clientProfileMobPath,
            Path clientsInfoPath, Path activityDensityProfileOut, Path tmpDirPath,
            boolean isDebug, Configuration conf)
            throws ClassNotFoundException, IOException, InterruptedException {
        WorkflowList wfList = new WorkflowList();
        Path popdenprofBtsprofPath = new Path(tmpDirPath, "popdenprof_btsprof");
        CosmosJob popdenprofBtsprofJob = CosmosJob.createReduceJob(conf,
                "PopdenJoinArrayProfile",
                SequenceFileInputFormat.class,
                PopdenJoinArrayProfileReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(popdenprofBtsprofJob, clientsInfoPath,
            clientProfileMobPath);
        FileOutputFormat.setOutputPath(popdenprofBtsprofJob,
                                       popdenprofBtsprofPath);
        popdenprofBtsprofJob.setDeleteOutputOnExit(!isDebug);
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
        
        Path populationDensityProfilePath = new Path(tmpDirPath,
                "population_density_profile");
        CosmosJob populationDensityProfileJob = CosmosJob.createReduceJob(conf,
                "PopdenCreateVector",
                SequenceFileInputFormat.class,
                PopdenCreateVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(populationDensityProfileJob,
                                      popdenBtsprofCountPath);
        FileOutputFormat.setOutputPath(populationDensityProfileJob,
                                       populationDensityProfilePath);
        populationDensityProfileJob.addDependentWorkflow(popdenBtsprofCountJob);
        wfList.add(populationDensityProfileJob);
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenProfileGetOut",
                    SequenceFileInputFormat.class,
                    PopdenProfileGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, populationDensityProfilePath);
            FileOutputFormat.setOutputPath(job, activityDensityProfileOut);
            job.addDependentWorkflow(populationDensityProfileJob);
            wfList.add(job);
        }
        return wfList;
    }
}
