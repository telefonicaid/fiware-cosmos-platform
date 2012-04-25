package es.tid.cosmos.mobility.mivs;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;

/**
 *
 * @author logc
 */
public final class MivsRunner {
    private MivsRunner() {
    }

    public static void run(Path viTelmonthBts, Path viClientFuseAcc,
                           Path tmpDir, boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path viTelmonthMobvars = new Path(tmpDir, "vi_telmonth_mobvars");
        {
            // Calculate individual variables by month
            ReduceJob job = ReduceJob.create(conf, "ActivityAreaByMonth",
                    SequenceFileInputFormat.class, ActivityAreaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viTelmonthBts);
            FileOutputFormat.setOutputPath(job, viTelmonthMobvars);
            job.waitForCompletion(true);
        }
        
        Path viClientFuse = new Path(tmpDir, "vi_client_fuse");
        {
            // Fuse in a set all user info
            ReduceJob job = ReduceJob.create(conf, "FusionTotalVars",
                    SequenceFileInputFormat.class, FusionTotalVarsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viTelmonthMobvars);
            FileOutputFormat.setOutputPath(job, viClientFuse);
            job.waitForCompletion(true);
        }

        Path viTelmonthBtsAcc = new Path(tmpDir, "vi_telmonth_bts_acc");
        {
            // Delete months
            MapJob job = MapJob.create(conf, "DeletePeriod",
                    SequenceFileInputFormat.class, DeletePeriodMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viTelmonthBts);
            FileOutputFormat.setOutputPath(job, viTelmonthBtsAcc);
            job.waitForCompletion(true);
        }

        Path viTelmonthMobvarsAcc = new Path(tmpDir, "vi_telmonth_mobvars_acc");
        {
            // Calculate individual variables for every month
            ReduceJob job = ReduceJob.create(conf, "ActivityAreaByMonth",
                    SequenceFileInputFormat.class, ActivityAreaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viTelmonthBtsAcc);
            FileOutputFormat.setOutputPath(job, viTelmonthMobvarsAcc);
            job.waitForCompletion(true);
        }

        {
            // Fuse in a set all user info
            ReduceJob job = ReduceJob.create(conf, "FusionTotalVars",
                    SequenceFileInputFormat.class, FusionTotalVarsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viTelmonthMobvarsAcc);
            FileOutputFormat.setOutputPath(job, viClientFuseAcc);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path viClientFuseText = new Path(tmpDir, "vi_client_fuse_text");
            {
                ReduceJob job = ReduceJob.create(conf, "IndVarsOut",
                        SequenceFileInputFormat.class,
                        IndVarsOutReducer.class,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, viClientFuse);
                FileOutputFormat.setOutputPath(job, viClientFuseText);
                job.waitForCompletion(true);
            }

            Path viClientFuseAccText = new Path(tmpDir,
                                                "vi_client_fuse_acc_text");
            {
                ReduceJob job = ReduceJob.create(conf, "IndVarsOutAcc",
                        SequenceFileInputFormat.class,
                        IndVarsOutAccReducer.class,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, viClientFuseAcc);
                FileOutputFormat.setOutputPath(job, viClientFuseAccText);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(viTelmonthMobvars, true);
            fs.delete(viTelmonthBtsAcc, true);
            fs.delete(viTelmonthMobvarsAcc, true);
        }
    }
}
