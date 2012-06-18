package es.tid.cosmos.mobility.preparing;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;

/**
 *
 * @author dmicol
 */
public class PreparingRunner {
    public PreparingRunner() {
    }

    public CosmosWorkflow run(Path tmpPath, Path cdrsMobPath,
            Path cdrsInfoPath, Path cdrsNoinfoPath, Path cellsPath,
            Path clientsBtsPath, Path btsCommsPath, Path cdrsNoBtsPath,
            Path viTelmonthBtsPath, boolean isDebug, Configuration conf)
        throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        CosmosJob cdrsInfoJob;
        {
            cdrsInfoJob = CosmosJob.createMapJob(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(cdrsInfoJob, cdrsMobPath);
            FileOutputFormat.setOutputPath(cdrsInfoJob, cdrsInfoPath);
            wfList.add(cdrsInfoJob);
        }
        
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsNoinfoPath);
            job.setDeleteOutputOnExit(!isDebug);
            wfList.add(job);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "JoinBtsNodeToNodeBts",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToNodeBtsReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, clientsBtsPath);
            job.setDeleteOutputOnExit(!isDebug);
            job.addDependentWorkflow(cdrsInfoJob);
            wfList.add(job);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "JoinBtsNodeToBtsDayRange",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToBtsDayRangeReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, btsCommsPath);
            job.setDeleteOutputOnExit(!isDebug);
            job.addDependentWorkflow(cdrsInfoJob);
            wfList.add(job);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "JoinBtsNodeToCdr",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToCdrReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, cdrsNoBtsPath);
            job.setDeleteOutputOnExit(!isDebug);
            job.addDependentWorkflow(cdrsInfoJob);
            wfList.add(job);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "JoinBtsNodeToTelMonthAndCell",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToTelMonthAndCellReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, viTelmonthBtsPath);
            job.addDependentWorkflow(cdrsInfoJob);
            wfList.add(job);
        }
        return wfList;
    }
}
