package es.tid.cosmos.mobility.activitydensity;

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
public final class ActivityDensityRunner {
    private ActivityDensityRunner() {
    }
    
    public static void run(Path clientProfileMobPath, Path populationDensityOutPath,
                           Path tmpDirPath, boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        Path popdenBtsPath = new Path(tmpDirPath, "popden_bts");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenSpreadArray",
                    SequenceFileInputFormat.class,
                    PopdenSpreadArrayReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientProfileMobPath);
            FileOutputFormat.setOutputPath(job, popdenBtsPath);
            job.waitForCompletion(true);
        }

        Path popdenBtsCountPath = new Path(tmpDirPath, "popden_bts_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenSumComms",
                    SequenceFileInputFormat.class,
                    PopdenSumCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenBtsPath);
            FileOutputFormat.setOutputPath(job, popdenBtsCountPath);
            job.waitForCompletion(true);
        }

        Path populationDensityPath = new Path(tmpDirPath, "population_density");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenCreateVector",
                    SequenceFileInputFormat.class,
                    PopdenCreateVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenBtsCountPath);
            FileOutputFormat.setOutputPath(job, populationDensityPath);
            job.waitForCompletion(true);
        }

        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenProfileGetOut",
                    SequenceFileInputFormat.class,
                    PopdenProfileGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, populationDensityPath);
            FileOutputFormat.setOutputPath(job, populationDensityOutPath);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(popdenBtsPath, true);
            fs.delete(popdenBtsCountPath, true);
        }
    }
}
