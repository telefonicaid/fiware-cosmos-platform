package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.CosmosJob;

/**
 *
 * @author dmicol
 */
public final class ParsingRunner {
    private ParsingRunner() {
    }

    public static void run(Path cdrsPath, Path cdrsMobPath,
                           Path cellsPath, Path cellsMobPath,
                           Path adjBtsPath, Path pairbtsAdjPath,
                           Path btsVectorTxtPath, Path btsComareaPath,
                           Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ParseCdrs",
                    TextInputFormat.class,
                    ParseCdrsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsPath);
            FileOutputFormat.setOutputPath(job, cdrsMobPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "ParseCells",
                    TextInputFormat.class,
                    ParseCellsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsPath);
            FileOutputFormat.setOutputPath(job, cellsMobPath);
            job.waitForCompletion(true);
        }
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "AdjParseAdjBts",
                    TextInputFormat.class,
                    AdjParseAdjBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, adjBtsPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "BorrarGetBtsComarea",
                    TextInputFormat.class,
                    BorrarGetBtsComareaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsVectorTxtPath);
            FileOutputFormat.setOutputPath(job, btsComareaPath);
            job.waitForCompletion(true);
        }
    }
}
