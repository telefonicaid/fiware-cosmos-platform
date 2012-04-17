package es.tid.cosmos.mobility.activityarea;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author losa
 */
public final class ActivityAreaRunner {
    private ActivityAreaRunner(Path viTelmonthBts, Path viClientFuseTxt,
                               Path viClientFuseAccTxt, Path tmpDir,
                               Configuration conf) throws Exception {
        Path viTelmonthMobvars = tmpDir.suffix("vi_telmonth_mobvars");
        {
            // Calculate individual variables by month
            ActivityAreaByMonthJob job = new ActivityAreaByMonthJob(conf);
            job.configure(viTelmonthBts, viTelmonthMobvars);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path viClientFuse = tmpDir.suffix("vi_client_fuse");
        {
            // Fuse in a set all user info
            FusionTotalVarsJob job = new FusionTotalVarsJob(conf);
            job.configure(viTelmonthMobvars, viClientFuse);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            // Extract to text file
            IndVarsOutJob job = new IndVarsOutJob(conf);
            job.configure(viClientFuse, viClientFuseTxt);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path viTelmonthBtsAcc = tmpDir.suffix("vi_telmonth_bts_acc");
        {
            // Delete months
            DeletePeriodJob job = new DeletePeriodJob(conf);
            job.configure(viTelmonthBts, viTelmonthBtsAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path viTelmonthMobvarsAcc = tmpDir.suffix("vi_telmonth_mobvars_acc");
        {
            // Calculate individual variables for every month
            ActivityAreaByMonthJob job = new ActivityAreaByMonthJob(conf);
            job.configure(viTelmonthBtsAcc, viTelmonthMobvarsAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path viClientFuseAcc = tmpDir.suffix("vi_client_fuse_acc");
        {
            // Fuse in a set all user info
            FusionTotalVarsJob job = new FusionTotalVarsJob(conf);
            job.configure(viTelmonthMobvarsAcc, viClientFuseAcc);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            // Extract to text file
            IndVarsOutJob job = new IndVarsOutJob(conf);
            job.configure(viClientFuseAcc, viClientFuseAccTxt);
        }

	  /* Delete unused:
           * vi_telmonth_mobvars
           * vi_telmonth_bts_mobvars
           * vi_telmonth_bts_acc
           * vi_telmonth_mobvars_acc
           * vi_telmonth_bts
           */
    }
}
