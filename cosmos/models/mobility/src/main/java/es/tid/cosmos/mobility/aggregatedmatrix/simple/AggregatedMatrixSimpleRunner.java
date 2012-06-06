package es.tid.cosmos.mobility.aggregatedmatrix.simple;

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
import es.tid.cosmos.mobility.itineraries.*;

/**
 *
 * @author dmicol
 */
public final class AggregatedMatrixSimpleRunner {
    private AggregatedMatrixSimpleRunner() {
    }
    
    public static void run(Path cdrsInfoPath, Path cellsPath,
                           Path matrixPairBtsTxtPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        FileSystem fs = FileSystem.get(conf);
        
        Path mtxClientbtsTimePath = new Path(tmpDirPath, "mtx_clientbts_time");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinJoinCellBts",
                    SequenceFileInputFormat.class,
                    ItinJoinCellBtsReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, mtxClientbtsTimePath);
            job.waitForCompletion(true);
        }

        Path mtxClientTimePath = new Path(tmpDirPath, "mtx_client_time");
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "MatrixSpreadNode",
                    SequenceFileInputFormat.class,
                    MatrixSpreadNodeMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientbtsTimePath);
            FileOutputFormat.setOutputPath(job, mtxClientTimePath);
            job.waitForCompletion(true);
        }
        
        Path mtxClientMovesPath = new Path(tmpDirPath, "mtx_client_moves");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ItinMoveClientPois",
                    SequenceFileInputFormat.class,
                    ItinMoveClientPoisReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientTimePath);
            FileOutputFormat.setOutputPath(job, mtxClientMovesPath);
            job.waitForCompletion(true);
        }

        Path mtxClientMovesRangesPath = new Path(tmpDirPath,
                                                 "mtx_client_moves_ranges");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinGetRanges",
                    SequenceFileInputFormat.class,
                    ItinGetRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientMovesPath);
            FileOutputFormat.setOutputPath(job, mtxClientMovesRangesPath);
            job.waitForCompletion(true);
        }

        Path mtxPbtsMovesRangesPath = new Path(tmpDirPath,
                                               "mtx_pbts_moves_ranges");
        {
            CosmosJob job = CosmosJob.createMapJob(conf,
                    "MatrixSpreadDistMovesByPair",
                    SequenceFileInputFormat.class,
                    MatrixSpreadDistMovesByPairMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientMovesRangesPath);
            FileOutputFormat.setOutputPath(job, mtxPbtsMovesRangesPath);
            job.waitForCompletion(true);
        }

        Path mtxPbtsMovesCountPath = new Path(tmpDirPath,
                                              "mtx_pbts_moves_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinCountRanges",
                    SequenceFileInputFormat.class,
                    ItinCountRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxPbtsMovesRangesPath);
            FileOutputFormat.setOutputPath(job, mtxPbtsMovesCountPath);
            job.waitForCompletion(true);
        }
        
        Path mtxPbtsMovesVectorBtsPath = new Path(tmpDirPath,
                                                  "mtx_pbts_moves_vector_bts");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ItinGetVector",
                    SequenceFileInputFormat.class,
                    ItinGetVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxPbtsMovesCountPath);
            FileOutputFormat.setOutputPath(job, mtxPbtsMovesVectorBtsPath);
            job.waitForCompletion(true);
        }

        Path mtxPairbtsVectorPath = new Path(tmpDirPath, "mtx_pairbts_vector");
        {
            CosmosJob job = CosmosJob.createMapJob(conf,
                    "MatrixSpreadVectorByPair",
                    SequenceFileInputFormat.class,
                    MatrixSpreadVectorByPairMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxPbtsMovesVectorBtsPath);
            FileOutputFormat.setOutputPath(job, mtxPairbtsVectorPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetOut",
                    SequenceFileInputFormat.class,
                    MatrixGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxPairbtsVectorPath);
            FileOutputFormat.setOutputPath(job, matrixPairBtsTxtPath);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            fs.delete(mtxClientbtsTimePath, isDebug);
            fs.delete(mtxClientTimePath, isDebug);
            fs.delete(mtxClientMovesPath, isDebug);
            fs.delete(mtxClientMovesRangesPath, isDebug);
            fs.delete(mtxPbtsMovesRangesPath, isDebug);
            fs.delete(mtxPbtsMovesCountPath, isDebug);
            fs.delete(mtxPairbtsVectorPath, isDebug);
            fs.delete(mtxPbtsMovesVectorBtsPath, isDebug);
        }
    }
}
