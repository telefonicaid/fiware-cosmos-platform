package es.tid.cosmos.mobility.labelling.client;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.util.ConvertCdrToMobDataJob;
import es.tid.cosmos.mobility.util.ConvertIntToMobDataJob;
import es.tid.cosmos.mobility.util.ExportClusterToTextJob;

/**
 *
 * @author dmicol
 */
public final class ClientLabellingRunner {
    private ClientLabellingRunner() {
    }
    
    public static void run(Path cdrsMobPath, Path clientsInfoFilteredPath,
                           Path centroidsPath, Path vectorClientClusterPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path cdrsMobDataPath = new Path(tmpDirPath, "cdrs_mob_data");
        { 
            ConvertCdrToMobDataJob job = new ConvertCdrToMobDataJob(conf);
            job.configure(cdrsMobPath, cdrsMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientsInfoFilteredMobDataPath = new Path(tmpDirPath,
                "clients_info_filtered_mob_data");
        { 
            ConvertIntToMobDataJob job = new ConvertIntToMobDataJob(conf);
            job.configure(clientsInfoFilteredPath,
                          clientsInfoFilteredMobDataPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path cdrsFilteredPath = new Path(tmpDirPath, "cdrs_filtered");
        {
            VectorFiltClientsJob job = new VectorFiltClientsJob(conf);
            job.configure(new Path[] { cdrsMobDataPath,
                                       clientsInfoFilteredMobDataPath },
                          cdrsFilteredPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        fs.delete(cdrsMobDataPath, true);
        fs.delete(clientsInfoFilteredMobDataPath, true);
        
        Path vectorSpreadNodedayhourPath = new Path(tmpDirPath,
                                                    "clients_info_spread");
        {
            VectorSpreadNodedayhourJob job = new VectorSpreadNodedayhourJob(
                    conf);
            job.configure(cdrsFilteredPath, vectorSpreadNodedayhourPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorGetNcomsNodedayhourPath = new Path(tmpDirPath,
                                                      "cliVec_numcoms");
        {
            VectorGetNcomsNodedayhourJob job = new VectorGetNcomsNodedayhourJob(
                    conf);
            job.configure(vectorSpreadNodedayhourPath,
                          vectorGetNcomsNodedayhourPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorCreateNodeDayhourPath = new Path(tmpDirPath, "cliVec_group");
        {
            VectorCreateNodeDayhourJob job = new VectorCreateNodeDayhourJob(
                    conf);
            job.configure(vectorGetNcomsNodedayhourPath,
                          vectorCreateNodeDayhourPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorFuseNodeDaygroupPath = new Path(tmpDirPath, "vector_client");
        {
            VectorFuseNodeDaygroupJob job = new VectorFuseNodeDaygroupJob(conf);
            job.configure(vectorCreateNodeDayhourPath,
                          vectorFuseNodeDaygroupPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientNormPath = new Path(tmpDirPath, "vector_client_norm");
        {
            VectorNormalizedJob job = new VectorNormalizedJob(conf);
            job.configure(vectorFuseNodeDaygroupPath, vectorClientNormPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        {
            ClusterClientGetMinDistanceJob job =
                    new ClusterClientGetMinDistanceJob(conf);
            job.configure(vectorClientNormPath, centroidsPath,
                          vectorClientClusterPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        if (isDebug) {
            Path vectorClientClusterTextPath = new Path(tmpDirPath,
                    "vector_client_cluster_text");
            {
                ExportClusterToTextJob job = new ExportClusterToTextJob(conf);
                job.configure(vectorClientClusterPath,
                              vectorClientClusterTextPath);
                if (!job.waitForCompletion(true)) {
                    throw new Exception("Failed to run " + job.getJobName());
                }
            }
        } else {
            fs.delete(cdrsFilteredPath, true);
            fs.delete(clientsInfoFilteredPath, true);
            fs.delete(vectorClientNormPath, true);
        }
    }
}
