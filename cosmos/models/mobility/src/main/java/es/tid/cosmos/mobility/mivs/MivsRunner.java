package es.tid.cosmos.mobility.mivs;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

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
        Path viTelmonthMobvars = new Path(tmpDir, "vi_telmonth_mobvars");
        {
            // Calculate individual variables by month
            ActivityAreaByMonthJob job = new ActivityAreaByMonthJob(conf);
            job.configure(viTelmonthBts, viTelmonthMobvars);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path viClientFuse = new Path(tmpDir, "vi_client_fuse");
        {
            // Fuse in a set all user info
            FusionTotalVarsJob job = new FusionTotalVarsJob(conf);
            job.configure(viTelmonthMobvars, viClientFuse);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path viTelmonthBtsAcc = new Path(tmpDir, "vi_telmonth_bts_acc");
        {
            // Delete months
            DeletePeriodJob job = new DeletePeriodJob(conf);
            job.configure(viTelmonthBts, viTelmonthBtsAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path viTelmonthMobvarsAcc = new Path(tmpDir, "vi_telmonth_mobvars_acc");
        {
            // Calculate individual variables for every month
            ActivityAreaByMonthJob job = new ActivityAreaByMonthJob(conf);
            job.configure(viTelmonthBtsAcc, viTelmonthMobvarsAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            // Fuse in a set all user info
            FusionTotalVarsJob job = new FusionTotalVarsJob(conf);
            job.configure(viTelmonthMobvarsAcc, viClientFuseAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path viClientFuseText = new Path(tmpDir, "vi_client_fuse_text");
            {
                IndVarsOutJob job = new IndVarsOutJob(conf);
                job.configure(viClientFuse, viClientFuseText);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }

            Path viClientFuseAccText = new Path(tmpDir,
                                                "vi_client_fuse_acc_text");
            {
                IndVarsOutAccJob job = new IndVarsOutAccJob(conf);
                job.configure(viClientFuseAcc, viClientFuseAccText);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        } else {
            FileSystem fs = FileSystem.get(conf);
            fs.deleteOnExit(viTelmonthMobvars);
            fs.deleteOnExit(viTelmonthBtsAcc);
            fs.deleteOnExit(viTelmonthMobvarsAcc);
        }
    }
}
