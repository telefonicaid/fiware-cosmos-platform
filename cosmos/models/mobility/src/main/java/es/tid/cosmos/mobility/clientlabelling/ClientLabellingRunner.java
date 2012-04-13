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
    
    public static void run(Path input, Path output, Path tmpDir,
                           Configuration conf) throws Exception {
        Path vectorSpreadNodedayhour = tmpDir.suffix("vector_spread_nodedayhour");
        {
            VectorSpreadNodedayhourJob job = new VectorSpreadNodedayhourJob(
                    conf);
            job.configure(input, vectorSpreadNodedayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorGetNcomsNodedayhour = tmpDir.suffix(
                "vector_get_ncoms_nodedayhour");
        {
            VectorGetNcomsNodedayhourJob job = new VectorGetNcomsNodedayhourJob(
                    conf);
            job.configure(vectorSpreadNodedayhour, vectorGetNcomsNodedayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorCreateNodeDayhour = tmpDir.suffix(
                "vector_create_node_dayhour");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(vectorGetNcomsNodedayhour, vectorCreateNodeDayhour);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorFuseNodeDaygroup = tmpDir.suffix("vector_fuse_node_daygroup");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(vectorCreateNodeDayhour, vectorFuseNodeDaygroup);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorFuseNodeDaygroup, output);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
