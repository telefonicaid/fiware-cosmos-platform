package es.tid.cosmos.mobility.populationdensity.profile;

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
import es.tid.cosmos.mobility.activitydensity.PopdenProfileGetOutReducer;
import es.tid.cosmos.mobility.activitydensity.PopdenSumCommsReducer;

/**
 *
 * @author ximo
 */
public class PopulationDensityProfileRunner {
    private PopulationDensityProfileRunner() {
    }
    
    public static void run(Path cdrsInfoPath, Path cellsPath,
                           Path clientProfilePath,
                           Path populationDensityProfileOut, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
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
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenDeleteDuplicates",
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
            CosmosJob job = CosmosJob.createReduceJob(conf, "PopdenJoinNodeInfoProfile",
                    SequenceFileInputFormat.class,
                    PopdenJoinNodeInfoProfileReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, denpobNodeinfoNodupPath,
                                          clientProfilePath);
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
                    PopdenSumCommsReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, popdenBtsprofCountPath);
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
            FileOutputFormat.setOutputPath(job, populationDensityProfileOut);
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
