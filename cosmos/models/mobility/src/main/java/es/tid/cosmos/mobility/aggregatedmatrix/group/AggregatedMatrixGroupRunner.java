package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.mobility.aggregatedmatrix.simple.MatrixGetOutReducer;

/**
 *
 * @author dmicol
 */
public class AggregatedMatrixGroupRunner {
    private AggregatedMatrixGroupRunner() {
    }
    
    public static CosmosWorkflow run(Path cdrsInfoPath, Path cellsGroupPath,
            Path matrixPairGroupTxtPath, Path tmpDirPath, boolean isDebug,
            Configuration conf) throws IOException, InterruptedException,
                                       ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        Path mtxClientstaTimePath = new Path(tmpDirPath, "mtx_clientsta_time");
        CosmosJob mtxClientstaTimeJob = CosmosJob.createReduceJob(conf,
                "MatrixJoinCellGroup",
                SequenceFileInputFormat.class,
                MatrixJoinCellGroupReducer.class,
                SequenceFileOutputFormat.class);
        mtxClientstaTimeJob.getConfiguration().set("cell_groups",
                                    cellsGroupPath.toString());
        FileInputFormat.setInputPaths(mtxClientstaTimeJob, cdrsInfoPath);
        FileOutputFormat.setOutputPath(mtxClientstaTimeJob,
                                       mtxClientstaTimePath);
        mtxClientstaTimeJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(mtxClientstaTimeJob);
        
        Path mtxClientMovesPath = new Path(tmpDirPath, "mtx_client_moves");
        CosmosJob mtxClientMovesJob = CosmosJob.createReduceJob(conf,
                "MatrixMoveClient",
                SequenceFileInputFormat.class,
                MatrixMoveClientReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxClientMovesJob, mtxClientstaTimePath);
        FileOutputFormat.setOutputPath(mtxClientMovesJob, mtxClientMovesPath);
        mtxClientMovesJob.setDeleteOutputOnExit(!isDebug);
        mtxClientMovesJob.addDependentWorkflow(mtxClientstaTimeJob);
        wfList.add(mtxClientMovesJob);
        
        Path mtxClientMovesRangesPath = new Path(tmpDirPath,
                                                 "mtx_client_moves_ranges");
        CosmosJob mtxClientMovesRangesJob = CosmosJob.createReduceJob(conf,
                "MatrixGetRanges",
                SequenceFileInputFormat.class,
                MatrixGetRangesReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxClientMovesRangesJob,
                                      mtxClientMovesPath);
        FileOutputFormat.setOutputPath(mtxClientMovesRangesJob,
                                       mtxClientMovesRangesPath);
        mtxClientMovesRangesJob.setDeleteOutputOnExit(!isDebug);
        mtxClientMovesRangesJob.addDependentWorkflow(mtxClientMovesJob);
        wfList.add(mtxClientMovesRangesJob);
        
        Path mtxGroupMovesCountPath = new Path(tmpDirPath,
                                               "mtx_group_moves_count");
        CosmosJob mtxGroupMovesCountJob = CosmosJob.createReduceJob(conf,
                "MatrixCountRanges",
                SequenceFileInputFormat.class,
                MatrixCountRangesReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxGroupMovesCountJob,
                                      mtxClientMovesRangesPath);
        FileOutputFormat.setOutputPath(mtxGroupMovesCountJob,
                                       mtxGroupMovesCountPath);
        mtxGroupMovesCountJob.setDeleteOutputOnExit(!isDebug);
        mtxGroupMovesCountJob.addDependentWorkflow(mtxClientMovesRangesJob);
        wfList.add(mtxGroupMovesCountJob);

        Path mtxGroupMovesVectorBtsPath = new Path(tmpDirPath,
                "mtx_group_moves_vector_bts");
        CosmosJob mtxGroupMovesVectorBtsJob = CosmosJob.createReduceJob(conf,
                "MatrixGetVector",
                SequenceFileInputFormat.class,
                MatrixGetVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxGroupMovesVectorBtsJob,
                                      mtxGroupMovesCountPath);
        FileOutputFormat.setOutputPath(mtxGroupMovesVectorBtsJob,
                                       mtxGroupMovesVectorBtsPath);
        mtxGroupMovesVectorBtsJob.setDeleteOutputOnExit(!isDebug);
        mtxGroupMovesVectorBtsJob.addDependentWorkflow(mtxGroupMovesCountJob);
        wfList.add(mtxGroupMovesVectorBtsJob);

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetOut",
                    SequenceFileInputFormat.class,
                    MatrixGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxGroupMovesVectorBtsPath);
            FileOutputFormat.setOutputPath(job, matrixPairGroupTxtPath);
            job.addDependentWorkflow(mtxGroupMovesVectorBtsJob);
            wfList.add(job);
        }
        
        return wfList;
    }
}
