package es.tid.cosmos.mobility.btslabelling;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.clientlabelling.VectorCreateNodeDayhourJob;
import es.tid.cosmos.mobility.clientlabelling.VectorFuseNodeDaygroupJob;
import es.tid.cosmos.mobility.clientlabelling.VectorNormalizedJob;

/**
 *
 * @author dmicol
 */
public final class BtsLabellingRunner {
    private BtsLabellingRunner() {
    }
    
    public static void run(Path btsCommsPath, Path vectorBtsClusterPath,
                           Path tmpDir, Configuration conf) throws Exception {
        Path btsCountsPath = tmpDir.suffix("bts_counts");
        {
            VectorFilterBtsJob job = new VectorFilterBtsJob(conf);
            job.configure(btsCommsPath, btsCountsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path btsSumComsPath = tmpDir.suffix("bts_sum_coms");
        {
            VectorSumComsBtsJob job = new VectorSumComsBtsJob(conf);
            job.configure(btsCountsPath, btsSumComsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path btsDayhourPath = tmpDir.suffix("bts_dayhour");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(btsSumComsPath, btsDayhourPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsPath = tmpDir.suffix("vector_bts");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(btsDayhourPath, vectorBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsNormPath = tmpDir.suffix("vector_bts_norm");
        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorBtsPath, vectorBtsNormPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsClusterSinfiltPath = tmpDir.suffix(
                "vector_bts_cluster_sinfilt");
        {
            ClusterBtsGetMinDistanceJob job = new ClusterBtsGetMinDistanceJob(
                    conf);
            job.configure(vectorBtsNormPath, vectorBtsClusterSinfiltPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            FilterBtsVectorJob job = new FilterBtsVectorJob(conf);
            job.configure(vectorBtsClusterSinfiltPath, vectorBtsClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
