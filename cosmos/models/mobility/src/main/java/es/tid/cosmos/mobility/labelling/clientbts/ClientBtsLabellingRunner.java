package es.tid.cosmos.mobility.labelling.clientbts;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.labelling.client.VectorCreateNodeDayhourJob;
import es.tid.cosmos.mobility.labelling.client.VectorFuseNodeDaygroupJob;
import es.tid.cosmos.mobility.labelling.client.VectorNormalizedJob;
import es.tid.cosmos.mobility.util.ConvertBtsCounterToMobDataByTwoIntJob;
import es.tid.cosmos.mobility.util.ExportClusterToTextByTwoIntJob;
import es.tid.cosmos.mobility.util.SetMobDataInputIdByTwoIntJob;

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
        FileSystem fs = FileSystem.get(conf);
        
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

        Path clientsbtsSumMobDataPath = new Path(tmpDirPath,
                                                 "clientsbts_sum_mob_data");
        {
            ConvertBtsCounterToMobDataByTwoIntJob job =
                    new ConvertBtsCounterToMobDataByTwoIntJob(conf);
            job.configure(clientsbtsSumPath, clientsbtsSumMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsbtsSumMobDataWithInputIdPath = new Path(tmpDirPath,
                "clientsbts_sum_mob_data_with_input_id");
        {
            SetMobDataInputIdByTwoIntJob job = new SetMobDataInputIdByTwoIntJob(
                    conf);
            job.configure(clientsbtsSumMobDataPath, 0,
                          clientsbtsSumMobDataWithInputIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(clientsbtsSumMobDataPath, true);
        
        Path clientsRepbtsMobDataPath = new Path(tmpDirPath,
                                                 "clients_repbts_mob_data");
        {
            ConvertBtsCounterToMobDataByTwoIntJob job =
                    new ConvertBtsCounterToMobDataByTwoIntJob(conf);
            job.configure(clientsRepbtsPath, clientsRepbtsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsRepbtsMobDataWithInputIdPath = new Path(tmpDirPath,
                "clients_repbts_mob_data_with_input_id");
        {
            SetMobDataInputIdByTwoIntJob job = new SetMobDataInputIdByTwoIntJob(
                    conf);
            job.configure(clientsRepbtsMobDataPath, 1,
                          clientsRepbtsMobDataWithInputIdPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(clientsRepbtsMobDataPath, true);
        
        Path clientsbtsRepbtsPath = new Path(tmpDirPath, "clientsbts_repbts");
        {
            VectorFiltClientbtsJob job = new VectorFiltClientbtsJob(conf);
            job.configure(new Path[] { clientsbtsSumMobDataWithInputIdPath,
                                       clientsRepbtsMobDataWithInputIdPath },
                          clientsbtsRepbtsPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(clientsbtsSumMobDataWithInputIdPath, true);
        fs.delete(clientsRepbtsMobDataWithInputIdPath, true);
        
        Path clientsbtsGroupPath = new Path(tmpDirPath, "clientsbts_group");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(clientsbtsRepbtsPath, clientsbtsGroupPath);
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
                ExportClusterToTextByTwoIntJob job =
                        new ExportClusterToTextByTwoIntJob(conf);
                job.configure(vectorClientbtsClusterPath,
                              vectorClientbtsClusterTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        } else {
            fs.delete(clientsbtsSpreadPath, true);
            fs.delete(clientsbtsSumPath, true);
            fs.delete(clientsbtsRepbtsPath, true);
            fs.delete(clientsbtsGroupPath, true);
            fs.delete(vectorClientbtsNormPath, true);
        }
    }
}
