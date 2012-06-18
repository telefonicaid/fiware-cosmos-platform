package es.tid.cosmos.mobility.aggregatedmatrix.simple;

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
import es.tid.cosmos.mobility.itineraries.*;

/**
 *
 * @author dmicol
 */
public class AggregatedMatrixSimpleRunner {
    public AggregatedMatrixSimpleRunner() {
    }
    
    public CosmosWorkflow run(Path cdrsInfoPath, Path cellsPath,
                              Path matrixPairBtsTxtPath, Path tmpDirPath,
                              boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        
        Path mtxClientbtsTimePath = new Path(tmpDirPath, "mtx_clientbts_time");
        CosmosJob mtxClientbtsTimeJob = CosmosJob.createReduceJob(conf,
                "ItinJoinCellBts",
                SequenceFileInputFormat.class,
                ItinJoinCellBtsReducer.class,
                SequenceFileOutputFormat.class);
        mtxClientbtsTimeJob.getConfiguration().set("cells", cellsPath.toString());
        FileInputFormat.setInputPaths(mtxClientbtsTimeJob, cdrsInfoPath);
        FileOutputFormat.setOutputPath(mtxClientbtsTimeJob, mtxClientbtsTimePath);
        mtxClientbtsTimeJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(mtxClientbtsTimeJob);

        Path mtxClientTimePath = new Path(tmpDirPath, "mtx_client_time");
        CosmosJob mtxClientTimeJob = CosmosJob.createMapJob(conf, "MatrixSpreadNode",
                SequenceFileInputFormat.class,
                MatrixSpreadNodeMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxClientTimeJob, mtxClientbtsTimePath);
        FileOutputFormat.setOutputPath(mtxClientTimeJob, mtxClientTimePath);
        mtxClientTimeJob.setDeleteOutputOnExit(!isDebug);
        mtxClientTimeJob.addDependentWorkflow(mtxClientbtsTimeJob);
        wfList.add(mtxClientTimeJob);
        
        Path mtxClientMovesPath = new Path(tmpDirPath, "mtx_client_moves");
        CosmosJob mtxClientMovesJob = CosmosJob.createReduceJob(conf,
                "ItinMoveClientPois",
                SequenceFileInputFormat.class,
                ItinMoveClientPoisReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxClientMovesJob, mtxClientTimePath);
        FileOutputFormat.setOutputPath(mtxClientMovesJob, mtxClientMovesPath);
        mtxClientMovesJob.setDeleteOutputOnExit(!isDebug);
        mtxClientMovesJob.addDependentWorkflow(mtxClientTimeJob);
        wfList.add(mtxClientMovesJob);

        Path mtxClientMovesRangesPath = new Path(tmpDirPath,
                                                 "mtx_client_moves_ranges");
        CosmosJob mtxClientMovesRangesJob = CosmosJob.createReduceJob(conf,
                "ItinGetRanges",
                SequenceFileInputFormat.class,
                ItinGetRangesReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxClientMovesRangesJob, mtxClientMovesPath);
        FileOutputFormat.setOutputPath(mtxClientMovesRangesJob,
                                       mtxClientMovesRangesPath);
        mtxClientMovesRangesJob.setDeleteOutputOnExit(!isDebug);
        mtxClientMovesRangesJob.addDependentWorkflow(mtxClientMovesJob);
        wfList.add(mtxClientMovesRangesJob);

        Path mtxPbtsMovesRangesPath = new Path(tmpDirPath,
                                               "mtx_pbts_moves_ranges");
        CosmosJob mtxPbtsMovesRangesJob = CosmosJob.createMapJob(conf,
                "MatrixSpreadDistMovesByPair",
                SequenceFileInputFormat.class,
                MatrixSpreadDistMovesByPairMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxPbtsMovesRangesJob,
                                      mtxClientMovesRangesPath);
        FileOutputFormat.setOutputPath(mtxPbtsMovesRangesJob,
                                       mtxPbtsMovesRangesPath);
        mtxPbtsMovesRangesJob.setDeleteOutputOnExit(!isDebug);
        mtxPbtsMovesRangesJob.addDependentWorkflow(mtxClientMovesRangesJob);
        wfList.add(mtxPbtsMovesRangesJob);

        Path mtxPbtsMovesCountPath = new Path(tmpDirPath,
                                              "mtx_pbts_moves_count");
        CosmosJob mtxPbtsMovesCountJob = CosmosJob.createReduceJob(conf, 
                 "ItinCountRanges",
                SequenceFileInputFormat.class,
                ItinCountRangesReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxPbtsMovesCountJob,
                                      mtxPbtsMovesRangesPath);
        FileOutputFormat.setOutputPath(mtxPbtsMovesCountJob,
                                       mtxPbtsMovesCountPath);
        mtxPbtsMovesCountJob.setDeleteOutputOnExit(!isDebug);
        mtxPbtsMovesCountJob.addDependentWorkflow(mtxPbtsMovesRangesJob);
        wfList.add(mtxPbtsMovesCountJob);
        
        Path mtxPbtsMovesVectorBtsPath = new Path(tmpDirPath,
                                                  "mtx_pbts_moves_vector_bts");
        CosmosJob mtxPbtsMovesVectorBtsJob = CosmosJob.createReduceJob(conf, "ItinGetVector",
                SequenceFileInputFormat.class,
                ItinGetVectorReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxPbtsMovesVectorBtsJob,
                                      mtxPbtsMovesCountPath);
        FileOutputFormat.setOutputPath(mtxPbtsMovesVectorBtsJob,
                                       mtxPbtsMovesVectorBtsPath);
        mtxPbtsMovesVectorBtsJob.setDeleteOutputOnExit(!isDebug);
        mtxPbtsMovesVectorBtsJob.addDependentWorkflow(mtxPbtsMovesCountJob);
        wfList.add(mtxPbtsMovesVectorBtsJob);

        Path mtxPairbtsVectorPath = new Path(tmpDirPath, "mtx_pairbts_vector");
        CosmosJob mtxPairbtsVectorJob = CosmosJob.createMapJob(conf,
                "MatrixSpreadVectorByPair",
                SequenceFileInputFormat.class,
                MatrixSpreadVectorByPairMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(mtxPairbtsVectorJob, mtxPbtsMovesVectorBtsPath);
        FileOutputFormat.setOutputPath(mtxPairbtsVectorJob, mtxPairbtsVectorPath);
        mtxPairbtsVectorJob.setDeleteOutputOnExit(!isDebug);
        mtxPairbtsVectorJob.addDependentWorkflow(mtxPbtsMovesVectorBtsJob);
        wfList.add(mtxPairbtsVectorJob);

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "MatrixGetOut",
                    SequenceFileInputFormat.class,
                    MatrixGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, mtxPairbtsVectorPath);
            FileOutputFormat.setOutputPath(job, matrixPairBtsTxtPath);
            job.addDependentWorkflow(mtxPairbtsVectorJob);
            wfList.add(job);
        }
        
        return wfList;
    }
}
