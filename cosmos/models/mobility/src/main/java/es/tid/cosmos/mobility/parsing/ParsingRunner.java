package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;

/**
 *
 * @author dmicol
 */
public final class ParsingRunner {
    private ParsingRunner() {
    }

    public static CosmosWorkflow run(Path cdrsPath, Path cdrsMobPath,
            Path cellsPath, Path cellsMobPath, Path adjBtsPath,
            Path pairbtsAdjPath, Path btsVectorTxtPath, Path btsComareaPath,
            Path clientsInfoPath, Path clientsInfoMobPath, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wf = new WorkflowList();
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "ParseCdrs",
                    TextInputFormat.class,
                    ParseCdrMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsPath);
            FileOutputFormat.setOutputPath(job, cdrsMobPath);
            wf.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(conf, "ParseCells",
                    TextInputFormat.class,
                    ParseCellMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsPath);
            FileOutputFormat.setOutputPath(job, cellsMobPath);
            wf.add(job);
        }
        
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "AdjParseAdjBts",
                    TextInputFormat.class,
                    AdjParseAdjBtsMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, adjBtsPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjPath);
            wf.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(conf, "BorrarGetBtsComarea",
                    TextInputFormat.class,
                    BorrarGetBtsComareaMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsVectorTxtPath);
            FileOutputFormat.setOutputPath(job, btsComareaPath);
            wf.add(job);
        }
        
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "ParserClientsInfo",
                    TextInputFormat.class,
                    ParseClientProfileMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoPath);
            FileOutputFormat.setOutputPath(job, clientsInfoMobPath);
            wf.add(job);
        }
        
        return wf;
    }
}
