package es.tid.cosmos.mobility.clientlabelling;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class ClientLabellingRunner {
    private ClientLabellingRunner() {
    }
    
    public static void run(Path cdrsMobPath, Path clientsInfoFilteredPath,
                           Path vectorClientClusterPath, Path tmpDir,
                           Configuration conf) throws Exception {
        Path vectorSpreadNodedayhour = new Path(tmpDir, "clients_info_spread");
        {
            VectorSpreadNodedayhourJob job = new VectorSpreadNodedayhourJob(
                    conf);
            job.configure(input, vectorSpreadNodedayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorGetNcomsNodedayhour = new Path(tmpDir, "cliVec_numcoms");
        {
            VectorGetNcomsNodedayhourJob job = new VectorGetNcomsNodedayhourJob(
                    conf);
            job.configure(vectorSpreadNodedayhour, vectorGetNcomsNodedayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorCreateNodeDayhour = new Path(tmpDir, "cliVec_group");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(vectorGetNcomsNodedayhour, vectorCreateNodeDayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorFuseNodeDaygroup = new Path(tmpDir, "vector_client");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(vectorCreateNodeDayhour, vectorFuseNodeDaygroup);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorFuseNodeDaygroup, vectorClientClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
