package es.tid.cosmos.mobility.labelling.client;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.MapReduceJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.util.*;

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
            ReduceJob job = ReduceJob.create(conf, "ConvertCdrToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCdrToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsMobDataPath);
            job.waitForCompletion(true);
        }

        Path clientsInfoFilteredMobDataPath = new Path(tmpDirPath,
                "clients_info_filtered_mob_data");
        { 
            ReduceJob job = ReduceJob.create(conf, "ConvertIntToMobData",
                    SequenceFileInputFormat.class,
                    ConvertIntToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoFilteredPath);
            FileOutputFormat.setOutputPath(job, clientsInfoFilteredMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path cdrsFilteredPath = new Path(tmpDirPath, "cdrs_filtered");
        {
            MapReduceJob job = MapReduceJob.create(conf, "VectorFiltClients",
                    SequenceFileInputFormat.class,
                    VectorFiltClientsMapper.class,
                    VectorFiltClientsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                cdrsMobDataPath, clientsInfoFilteredMobDataPath });
            FileOutputFormat.setOutputPath(job, cdrsFilteredPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(cdrsMobDataPath, true);
        fs.delete(clientsInfoFilteredMobDataPath, true);
        
        Path vectorSpreadNodedayhourPath = new Path(tmpDirPath,
                                                    "clients_info_spread");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorSpreadNodedayhour",
                    SequenceFileInputFormat.class,
                    VectorSpreadNodedayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsFilteredPath);
            FileOutputFormat.setOutputPath(job, vectorSpreadNodedayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorGetNcomsNodedayhourPath = new Path(tmpDirPath,
                                                      "cliVec_numcoms");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorGetNcomsNodedayhour",
                    SequenceFileInputFormat.class,
                    VectorGetNcomsNodedayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorSpreadNodedayhourPath);
            FileOutputFormat.setOutputPath(job, vectorGetNcomsNodedayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorCreateNodeDayhourPath = new Path(tmpDirPath, "cliVec_group");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorCreateNodeDayhour",
                    SequenceFileInputFormat.class,
                    VectorCreateNodeDayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorGetNcomsNodedayhourPath);
            FileOutputFormat.setOutputPath(job, vectorCreateNodeDayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorFuseNodeDaygroupPath = new Path(tmpDirPath, "vector_client");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorFuseNodeDaygroup",
                    SequenceFileInputFormat.class,
                    VectorFuseNodeDaygroupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorCreateNodeDayhourPath);
            FileOutputFormat.setOutputPath(job, vectorFuseNodeDaygroupPath);
            job.waitForCompletion(true);
        }

        Path vectorClientNormPath = new Path(tmpDirPath, "vector_client_norm");
        {
            ReduceJob job = ReduceJob.create(conf, "VectorNormalized",
                    SequenceFileInputFormat.class,
                    VectorNormalizedReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorFuseNodeDaygroupPath);
            FileOutputFormat.setOutputPath(job, vectorClientNormPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf,
                    "ClusterClientGetMinDistance",
                    SequenceFileInputFormat.class,
                    ClusterClientGetMinDistanceReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("centroids", centroidsPath.toString());
            FileInputFormat.setInputPaths(job, vectorClientNormPath);
            FileOutputFormat.setOutputPath(job, vectorClientClusterPath);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path vectorClientClusterTextPath = new Path(tmpDirPath,
                    "vector_client_cluster_text");
            {
                ReduceJob job = ReduceJob.create(conf,"ExportClusterToText",
                        SequenceFileInputFormat.class,
                        ExportClusterToTextReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, vectorClientClusterPath);
                FileOutputFormat.setOutputPath(job, vectorClientClusterTextPath);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(cdrsFilteredPath, true);
            fs.delete(clientsInfoFilteredPath, true);
            fs.delete(vectorClientNormPath, true);
        }
    }
}
