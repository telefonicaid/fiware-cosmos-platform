package es.tid.cosmos.mobility.itineraries;

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

/**
 *
 * @author dmicol
 */
public final class ItinerariesRunner {
    private ItinerariesRunner() {
    }
    
    public static void run(Path cellsPath, Path cdrsInfoPath,
                           Path pointsOfInterestIdPath,
                           Path clientItinerariesTxtPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        Path itClientbtsTimePath = new Path(tmpDirPath, "it_clientbts_time");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinJoinCellBts",
                    SequenceFileInputFormat.class,
                    ItinJoinCellBtsReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, itClientbtsTimePath);
            job.waitForCompletion(true);
        }

        Path itClientpoiTimePath = new Path(tmpDirPath, "it_clientpoi_time");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinFilterPois",
                    SequenceFileInputFormat.class,
                    ItinFilterPoisReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { itClientbtsTimePath,
                pointsOfInterestIdPath });
            FileOutputFormat.setOutputPath(job, itClientpoiTimePath);
            job.waitForCompletion(true);
        }

        Path itClientMovesPath = new Path(tmpDirPath, "it_client_moves");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinMoveClientPois",
                    SequenceFileInputFormat.class,
                    ItinMoveClientPoisReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientpoiTimePath);
            FileOutputFormat.setOutputPath(job, itClientMovesPath);
            job.waitForCompletion(true);
        }

        Path itClientMovesRangesPath = new Path(tmpDirPath,
                                                "it_client_moves_ranges");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinGetRanges",
                    SequenceFileInputFormat.class,
                    ItinGetRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientMovesPath);
            FileOutputFormat.setOutputPath(job, itClientMovesRangesPath);
            job.waitForCompletion(true);
        }
        
        Path itClientMovesCountPath = new Path(tmpDirPath,
                                               "it_client_moves_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinCountRanges",
                    SequenceFileInputFormat.class,
                    ItinCountRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientMovesRangesPath);
            FileOutputFormat.setOutputPath(job, itClientMovesCountPath);
            job.waitForCompletion(true);
        }
        
        Path itClientMovesVectorPath = new Path(tmpDirPath,
                                                "it_client_moves_vector");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinGetVector",
                    SequenceFileInputFormat.class,
                    ItinGetVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientMovesCountPath);
            FileOutputFormat.setOutputPath(job, itClientMovesVectorPath);
            job.waitForCompletion(true);
        }

        Path itClientItinerariesPath = new Path(tmpDirPath,
                                                "it_client_itineraries");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinGetItinerary",
                    SequenceFileInputFormat.class,
                    ItinGetItineraryReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientMovesVectorPath);
            FileOutputFormat.setOutputPath(job, itClientItinerariesPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinItineraryOut",
                    SequenceFileInputFormat.class,
                    ItinItineraryOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, itClientItinerariesPath);
            FileOutputFormat.setOutputPath(job, clientItinerariesTxtPath);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(itClientbtsTimePath, true);
            fs.delete(itClientpoiTimePath, true);
            fs.delete(itClientMovesPath, true);
            fs.delete(itClientMovesRangesPath, true);
            fs.delete(itClientMovesCountPath, true);
            fs.delete(itClientMovesVectorPath, true);
        }
    }
}
