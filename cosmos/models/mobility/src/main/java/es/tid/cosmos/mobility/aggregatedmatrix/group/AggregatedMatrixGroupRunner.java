package es.tid.cosmos.mobility.aggregatedmatrix.group;

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
import es.tid.cosmos.mobility.aggregatedmatrix.simple.MatrixGetOutReducer;

/**
 *
 * @author dmicol
 */
public final class AggregatedMatrixGroupRunner {
    private AggregatedMatrixGroupRunner() {
    }
    
    public static void run(Path cdrsInfoPath, Path cellsGroupPath,
                           Path matrixPairGroupTxtPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        Path mtxClientstaTimePath = new Path(tmpDirPath, "mtx_clientsta_time");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "MatrixJoinCellGroup",
                    SequenceFileInputFormat.class,
                    MatrixJoinCellGroupReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cell_groups",
                                       cellsGroupPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, mtxClientstaTimePath);
            job.waitForCompletion(true);
        }
        
        Path mtxClientMovesPath = new Path(tmpDirPath, "mtx_client_moves");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixMoveClient",
                    SequenceFileInputFormat.class,
                    MatrixMoveClientReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientstaTimePath);
            FileOutputFormat.setOutputPath(job, mtxClientMovesPath);
            job.waitForCompletion(true);
        }
        
        Path mtxClientMovesRangesPath = new Path(tmpDirPath,
                                                 "mtx_client_moves_ranges");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetRanges",
                    SequenceFileInputFormat.class,
                    MatrixGetRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientMovesPath);
            FileOutputFormat.setOutputPath(job, mtxClientMovesRangesPath);
            job.waitForCompletion(true);
        }
        
        Path mtxGroupMovesCountPath = new Path(tmpDirPath,
                                               "mtx_group_moves_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixCountRanges",
                    SequenceFileInputFormat.class,
                    MatrixCountRangesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxClientMovesRangesPath);
            FileOutputFormat.setOutputPath(job, mtxGroupMovesCountPath);
            job.waitForCompletion(true);
        }

        Path mtxGroupMovesVectorBtsPath = new Path(tmpDirPath,
                "mtx_group_moves_vector_bts");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetVector",
                    SequenceFileInputFormat.class,
                    MatrixGetVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxGroupMovesCountPath);
            FileOutputFormat.setOutputPath(job, mtxGroupMovesVectorBtsPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetOut",
                    SequenceFileInputFormat.class,
                    MatrixGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxGroupMovesVectorBtsPath);
            FileOutputFormat.setOutputPath(job, matrixPairGroupTxtPath);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(mtxClientstaTimePath, true);
            fs.delete(mtxClientMovesPath, true);
            fs.delete(mtxClientMovesRangesPath, true);
            fs.delete(mtxGroupMovesCountPath, true);
            fs.delete(mtxGroupMovesVectorBtsPath, true);
        }
    }
}
