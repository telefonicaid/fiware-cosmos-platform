package es.tid.cosmos.mobility.parsing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.ReduceJob;

/**
 *
 * @author dmicol
 */
public final class ParsingRunner {
    private ParsingRunner() {
    }

    public static void run(Path cdrsPath, Path cdrsMobPath,
                           Path cellsPath, Path cellsMobPath,
                           Path pairbtsadjTxtPath, Path pairbtsAdjPath,
                           Path btsVectorTxtPath, Path btsComareaPath,
                           Configuration conf) throws Exception {
        {
            ReduceJob job = ReduceJob.create(conf, "ParseCdrs",
                    TextInputFormat.class, ParseCdrsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsPath);
            FileOutputFormat.setOutputPath(job, cdrsMobPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "ParseCells",
                    TextInputFormat.class, ParseCellsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsPath);
            FileOutputFormat.setOutputPath(job, cellsMobPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "AdjParseAdjBts",
                    TextInputFormat.class, AdjParseAdjBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pairbtsadjTxtPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "BorrarGetBtsComarea",
                    TextInputFormat.class, BorrarGetBtsComareaReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsVectorTxtPath);
            FileOutputFormat.setOutputPath(job, btsComareaPath);
            job.waitForCompletion(true);
        }
    }
}
