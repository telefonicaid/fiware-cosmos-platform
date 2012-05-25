package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.mobility.util.ExportClusterToTextReducer;

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
            throws IOException, InterruptedException, ClassNotFoundException {
        FileSystem fs = FileSystem.get(conf);
        
        Path cdrsFilteredPath = new Path(tmpDirPath, "cdrs_filtered");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorFiltClients",
                    SequenceFileInputFormat.class,
                    VectorFiltClientsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                cdrsMobPath, clientsInfoFilteredPath });
            FileOutputFormat.setOutputPath(job, cdrsFilteredPath);
            job.waitForCompletion(true);
        }
        
        Path vectorSpreadNodedayhourPath = new Path(tmpDirPath,
                                                    "clients_info_spread");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorSpreadNodedayhour",
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
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorGetNcomsNodedayhour",
                    SequenceFileInputFormat.class,
                    VectorGetNcomsNodedayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorSpreadNodedayhourPath);
            FileOutputFormat.setOutputPath(job, vectorGetNcomsNodedayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorCreateNodeDayhourPath = new Path(tmpDirPath, "cliVec_group");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorCreateNodeDayhour",
                    SequenceFileInputFormat.class,
                    VectorCreateNodeDayhourReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorGetNcomsNodedayhourPath);
            FileOutputFormat.setOutputPath(job, vectorCreateNodeDayhourPath);
            job.waitForCompletion(true);
        }

        Path vectorFuseNodeDaygroupPath = new Path(tmpDirPath, "vector_client");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorFuseNodeDaygroup",
                    SequenceFileInputFormat.class,
                    VectorFuseNodeDaygroupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorCreateNodeDayhourPath);
            FileOutputFormat.setOutputPath(job, vectorFuseNodeDaygroupPath);
            job.waitForCompletion(true);
        }

        Path vectorClientNormPath = new Path(tmpDirPath, "vector_client_norm");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "VectorNormalized",
                    SequenceFileInputFormat.class,
                    VectorNormalizedReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, vectorFuseNodeDaygroupPath);
            FileOutputFormat.setOutputPath(job, vectorClientNormPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
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
                CosmosJob job = CosmosJob.createReduceJob(conf,"ExportClusterToText",
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
