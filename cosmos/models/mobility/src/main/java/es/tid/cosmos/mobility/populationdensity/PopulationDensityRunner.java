package es.tid.cosmos.mobility.populationdensity;

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
import es.tid.cosmos.mobility.activitydensity.PopdenCreateVectorReducer;
import es.tid.cosmos.mobility.activitydensity.PopdenProfileGetOutReducer;
import es.tid.cosmos.mobility.activitydensity.PopdenSumCommsReducer;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenDeleteDuplicatesReducer;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenSpreadNodebtsdayhourReducer;

/**
 *
 * @author ximo
 */
public final class PopulationDensityRunner {
    private PopulationDensityRunner() {
    }
    
    public static void run(Path cdrsInfoPath, Path cellsPath,
                           Path clientProfileMobPath,
                           Path populationDensityOut, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws ClassNotFoundException, IOException, InterruptedException {
        Path denpobNodbtsdayhourPath = new Path(tmpDirPath,
                                                "denpob_nodbtsdayhour");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenSpreadNodebtsdayhour",
                    SequenceFileInputFormat.class,
                    PopdenSpreadNodebtsdayhourReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, denpobNodbtsdayhourPath);
            job.waitForCompletion(true);
        }

        Path denpobNodeinfoNodupPath = new Path(tmpDirPath,
                                                "denpob_nodeinfo_nodup");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenDeleteDuplicates",
                    SequenceFileInputFormat.class,
                    PopdenDeleteDuplicatesReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, denpobNodbtsdayhourPath);
            FileOutputFormat.setOutputPath(job, denpobNodeinfoNodupPath);
            job.waitForCompletion(true);
        }

        Path popdenprofBtsprofPath = new Path(tmpDirPath,
                                              "popdenprof_btsprof");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenJoinNodeInfoWithoutProfile",
                    SequenceFileInputFormat.class,
                    PopdenJoinNodeInfoWithoutProfileReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, denpobNodeinfoNodupPath,
                                          clientProfileMobPath);
            FileOutputFormat.setOutputPath(job, popdenprofBtsprofPath);
            job.waitForCompletion(true);
        }
        
        Path popdenBtsprofCountPath = new Path(tmpDirPath,
                                               "popden_btsprof_count");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenSumComms",
                    SequenceFileInputFormat.class,
                    PopdenSumCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenprofBtsprofPath);
            FileOutputFormat.setOutputPath(job, popdenBtsprofCountPath);
            job.waitForCompletion(true);
        }
        
        Path populationDensityPath = new Path(tmpDirPath,
                                              "population_density");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenSumComms",
                    SequenceFileInputFormat.class,
                    PopdenCreateVectorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenBtsprofCountPath);
            FileOutputFormat.setOutputPath(job, populationDensityPath);
            job.waitForCompletion(true);
        }
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PopdenProfileGetOut",
                    SequenceFileInputFormat.class,
                    PopdenProfileGetOutReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, populationDensityPath);
            FileOutputFormat.setOutputPath(job, populationDensityOut);
            job.waitForCompletion(true);
        }
        
        if (!isDebug) {
            FileSystem fs = FileSystem.get(conf);
            fs.delete(denpobNodbtsdayhourPath, true);
            fs.delete(denpobNodeinfoNodupPath, true);
            fs.delete(popdenBtsprofCountPath, true);
            fs.delete(populationDensityPath, true);
        }
    }
}
