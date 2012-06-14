package es.tid.cosmos.mobility.mivs;

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
 * @author logc
 */
public final class MivsRunner {
    private MivsRunner() {
    }

    public static CosmosWorkflow run(Path viTelmonthBts, Path viClientFuseAcc,
                           Path tmpDir, boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();       
        Path viTelmonthMobvars = new Path(tmpDir, "vi_telmonth_mobvars");
        CosmosJob viTelmonthMobvarsJob;
        {
            // Calculate individual variables by month
            viTelmonthMobvarsJob = CosmosJob.createReduceJob(conf, "ActivityAreaByMonth",
                    SequenceFileInputFormat.class,
                    ActivityAreaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(viTelmonthMobvarsJob, viTelmonthBts);
            FileOutputFormat.setOutputPath(viTelmonthMobvarsJob, viTelmonthMobvars);
            viTelmonthMobvarsJob.setDeleteOutputOnExit(!isDebug);
            wfList.add(viTelmonthMobvarsJob);
        }
        
        Path viClientFuse = new Path(tmpDir, "vi_client_fuse");
        CosmosJob viClientFuseJob;
        {
            // Fuse in a set all user info
            viClientFuseJob = CosmosJob.createReduceJob(conf, "FusionTotalVars",
                    SequenceFileInputFormat.class,
                    FusionTotalVarsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(viClientFuseJob, viTelmonthMobvars);
            FileOutputFormat.setOutputPath(viClientFuseJob, viClientFuse);
            viClientFuseJob.addDependentWorkflow(viTelmonthMobvarsJob);
            wfList.add(viClientFuseJob);
        }

        Path viTelmonthBtsAcc = new Path(tmpDir, "vi_telmonth_bts_acc");
        CosmosJob viTelmonthBtsAccJob;
        {
            // Delete months
            viTelmonthBtsAccJob = CosmosJob.createMapJob(conf, "DeletePeriod",
                    SequenceFileInputFormat.class,
                    DeletePeriodMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(viTelmonthBtsAccJob, viTelmonthBts);
            FileOutputFormat.setOutputPath(viTelmonthBtsAccJob, viTelmonthBtsAcc);
            viTelmonthBtsAccJob.setDeleteOutputOnExit(!isDebug);
            wfList.add(viTelmonthBtsAccJob);
        }

        Path viTelmonthMobvarsAcc = new Path(tmpDir, "vi_telmonth_mobvars_acc");
        CosmosJob viTelmonthMobvarsAccJob;
        {
            // Calculate individual variables for every month
            viTelmonthMobvarsAccJob = CosmosJob.createReduceJob(conf, "ActivityAreaByMonth",
                    SequenceFileInputFormat.class,
                    ActivityAreaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(viTelmonthMobvarsAccJob, viTelmonthBtsAcc);
            FileOutputFormat.setOutputPath(viTelmonthMobvarsAccJob, viTelmonthMobvarsAcc);
            viTelmonthMobvarsAccJob.addDependentWorkflow(viTelmonthBtsAccJob);
            viTelmonthMobvarsAccJob.setDeleteOutputOnExit(!isDebug);
            wfList.add(viTelmonthMobvarsAccJob);
        }

        CosmosJob viClientFuseAccJob;
        {
            // Fuse in a set all user info
            viClientFuseAccJob = CosmosJob.createReduceJob(conf, "FusionTotalVars",
                    SequenceFileInputFormat.class,
                    FusionTotalVarsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(viClientFuseAccJob, viTelmonthMobvarsAcc);
            FileOutputFormat.setOutputPath(viClientFuseAccJob, viClientFuseAcc);
            viClientFuseAccJob.addDependentWorkflow(viTelmonthMobvarsAccJob);
            wfList.add(viClientFuseAccJob);
        }

        if (isDebug) {
            Path viClientFuseText = new Path(tmpDir, "vi_client_fuse_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf, "IndVarsOut",
                        SequenceFileInputFormat.class,
                        IndVarsOutReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, viClientFuse);
                FileOutputFormat.setOutputPath(job, viClientFuseText);
                job.addDependentWorkflow(viClientFuseJob);
                wfList.add(job);
            }

            Path viClientFuseAccText = new Path(tmpDir,
                                                "vi_client_fuse_acc_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf, "IndVarsOutAcc",
                        SequenceFileInputFormat.class,
                        IndVarsOutAccReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, viClientFuseAcc);
                FileOutputFormat.setOutputPath(job, viClientFuseAccText);
                job.addDependentWorkflow(viClientFuseAccJob);
                wfList.add(job);
            }
        }
        
        return wfList;
    }
}
