package es.tid.cosmos.mobility.btslabelling;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.clientlabelling.VectorCreateNodeDayhourJob;
import es.tid.cosmos.mobility.clientlabelling.VectorFuseNodeDaygroupJob;
import es.tid.cosmos.mobility.clientlabelling.VectorNormalizedJob;
import es.tid.cosmos.mobility.util.ConvertBtsToMobDataJob;
import es.tid.cosmos.mobility.util.ConvertClusterToMobDataJob;

/**
 *
 * @author dmicol
 */
public final class BtsLabellingRunner {
    private BtsLabellingRunner() {
    }
    
    public static void run(Path btsCommsPath, Path btsComareaPath,
                           Path centroidsPath, Path vectorBtsClusterPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws Exception {
        Path btsCountsPath = new Path(tmpDirPath, "bts_counts");
        {
            VectorFilterBtsJob job = new VectorFilterBtsJob(conf);
            job.configure(btsCommsPath, btsCountsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path btsSumComsPath = new Path(tmpDirPath, "bts_sum_coms");
        {
            VectorSumComsBtsJob job = new VectorSumComsBtsJob(conf);
            job.configure(btsCountsPath, btsSumComsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path btsDayhourPath = new Path(tmpDirPath, "bts_dayhour");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(btsSumComsPath, btsDayhourPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsPath = new Path(tmpDirPath, "vector_bts");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(btsDayhourPath, vectorBtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsNormPath = new Path(tmpDirPath, "vector_bts_norm");
        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorBtsPath, vectorBtsNormPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsClusterSinfiltPath = new Path(tmpDirPath,
                "vector_bts_cluster_sinfilt");
        {
            ClusterBtsGetMinDistanceJob job = new ClusterBtsGetMinDistanceJob(
                    conf);
            job.configure(vectorBtsNormPath, centroidsPath,
                          vectorBtsClusterSinfiltPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorBtsClusterSinfiltMobDataPath = new Path(tmpDirPath,
                "vector_bts_cluster_sinfilt_mob_data");
        { 
            ConvertClusterToMobDataJob job = new ConvertClusterToMobDataJob(
                    conf);
            job.configure(vectorBtsClusterSinfiltPath,
                          vectorBtsClusterSinfiltMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path btsComareaMobDataPath = new Path(tmpDirPath,
                                              "bts_comarea_mob_data");
        { 
            ConvertBtsToMobDataJob job = new ConvertBtsToMobDataJob(conf);
            job.configure(btsComareaPath, btsComareaMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            FilterBtsVectorJob job = new FilterBtsVectorJob(conf);
            job.configure(new Path[] { vectorBtsClusterSinfiltMobDataPath,
                                       btsComareaMobDataPath },
                          vectorBtsClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path vectorBtsClusterTextPath = new Path(tmpDirPath,
                                                    "vector_bts_cluster_text");
            {
                ExportClusterClientMinDistanceToTextJob job =
                        new ExportClusterClientMinDistanceToTextJob(conf);
                job.configure(vectorBtsClusterPath, vectorBtsClusterTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        }
    }
}
