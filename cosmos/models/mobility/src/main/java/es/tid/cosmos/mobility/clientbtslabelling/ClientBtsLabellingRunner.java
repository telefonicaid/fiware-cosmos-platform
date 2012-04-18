package es.tid.cosmos.mobility.clientbtslabelling;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.btslabelling.*;
import es.tid.cosmos.mobility.clientlabelling.VectorCreateNodeDayhourJob;
import es.tid.cosmos.mobility.clientlabelling.VectorFuseNodeDaygroupJob;
import es.tid.cosmos.mobility.clientlabelling.VectorNormalizedJob;

/**
 *
 * @author dmicol
 */
public final class ClientBtsLabellingRunner {
    private ClientBtsLabellingRunner() {
    }
    
    public static void run(Path clientsInfoPath, Path clientsRepbtsPath,
                           Path centroidsPath, Path pointsOfInterestTempPath,
                           Path vectorClientbtsClusterPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        Path clientsbtsSpreadPath = new Path(tmpDirPath, "clientsbts_spread");
        {
            VectorSpreadNodbtsJob job = new VectorSpreadNodbtsJob(conf);
            job.configure(clientsInfoPath, clientsbtsSpreadPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsbtsSumPath = new Path(tmpDirPath, "clientsbts_sum");
        {
            VectorSumGroupcommsJob job = new VectorSumGroupcommsJob(conf);
            job.configure(clientsbtsSpreadPath, clientsbtsSumPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsbtsRpbtsPath = new Path(tmpDirPath, "clientsbts_repbts");
        {
            VectorFiltClientbtsJob job = new VectorFiltClientbtsJob(conf);
            job.configure(new Path[] { clientsbtsSumPath, clientsRepbtsPath },
                          clientsbtsRpbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientsbtsGroupPath = new Path(tmpDirPath, "clientsbts_group");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(clientsbtsRpbtsPath, clientsbtsGroupPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientbtsPath = new Path(tmpDirPath, "vector_clientbts");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(clientsbtsGroupPath, vectorClientbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientbtsNormPath = new Path(tmpDirPath,
                                                "vector_clientbts_norm");
        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorClientbtsPath, vectorClientbtsNormPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            ClusterClientBtsGetMinDistanceToPoiJob job =
                    new ClusterClientBtsGetMinDistanceToPoiJob(conf);
            job.configure(vectorClientbtsNormPath, centroidsPath,
                          pointsOfInterestTempPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        {
            ClusterClientBtsGetMinDistanceToClusterJob job =
                    new ClusterClientBtsGetMinDistanceToClusterJob(conf);
            job.configure(vectorClientbtsNormPath, centroidsPath,
                          vectorClientbtsClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path vectorClientbtsClusterTextPath = new Path(tmpDirPath,
                    "vector_clientbts_cluster_text");
            {
                ExportClusterClientMinDistanceToTextJob job =
                        new ExportClusterClientMinDistanceToTextJob(conf);
                job.configure(vectorClientbtsClusterPath,
                              vectorClientbtsClusterTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        }
    }
}
