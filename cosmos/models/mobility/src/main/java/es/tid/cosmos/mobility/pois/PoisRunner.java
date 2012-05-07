package es.tid.cosmos.mobility.pois;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.util.*;

/**
 *
 * @author dmicol
 */
public final class PoisRunner {
    private PoisRunner() {
    }
    
    public static void run(Path tmpDirPath, Path clientsBtsPath,
                           Path clientsInfoPath, Path cdrsNoinfoPath,
                           Path cdrsNoBtsPath, Path clientsInfoFilteredPath,
                           Path clientsRepbtsPath, boolean isDebug,
                           Configuration conf) throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path clientsBtscounterPath = new Path(tmpDirPath, "clients_btscounter");
        {
            ReduceJob job = ReduceJob.create(conf, "NodeBtsCounter",
                    SequenceFileInputFormat.class,
                    NodeBtsCounterReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsBtsPath);
            FileOutputFormat.setOutputPath(job, clientsBtscounterPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "NodeMobInfo",
                    SequenceFileInputFormat.class,
                    NodeMobInfoReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsBtscounterPath);
            FileOutputFormat.setOutputPath(job, clientsInfoPath);
            job.waitForCompletion(true);
        }

        Path clientsInfoSpreadPath = new Path(tmpDirPath, "clients_info_spread");
        {
            MapJob job = MapJob.create(conf, "RepbtsSpreadNodebts",
                    SequenceFileInputFormat.class,
                    RepbtsSpreadNodebtsMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoPath);
            FileOutputFormat.setOutputPath(job, clientsInfoSpreadPath);
            job.waitForCompletion(true);
        }

        Path clientsInfoAggbybtsPath = new Path(tmpDirPath,
                                                "clients_info_aggbybts");
        {
            ReduceJob job = ReduceJob.create(conf, "RepbtsAggbybts",
                    SequenceFileInputFormat.class,
                    RepbtsAggbybtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoSpreadPath);
            FileOutputFormat.setOutputPath(job, clientsInfoAggbybtsPath);
            job.waitForCompletion(true);
        }
        
        Path repbtsAggbybtsMobDataPath = new Path(tmpDirPath,
                                                  "repbts_aggbybts_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertNodeBtsDayToMobData",
                    SequenceFileInputFormat.class,
                    ConvertNodeBtsDayToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoAggbybtsPath);
            FileOutputFormat.setOutputPath(job, repbtsAggbybtsMobDataPath);
            job.waitForCompletion(true);
        }

        Path cdrsNoinfoMobDataPath = new Path(tmpDirPath,
                                              "cdrs_noinfo_mob_data");
        { 
            ReduceJob job = ReduceJob.create(conf, "ConvertCdrToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCdrToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsNoinfoPath);
            FileOutputFormat.setOutputPath(job, cdrsNoinfoMobDataPath);
            job.waitForCompletion(true);
        }

        Path cdrsNoBtsMobDataPath = new Path(tmpDirPath, "cdrs_nobts_mob_data");
        { 
            ReduceJob job = ReduceJob.create(conf, "ConvertCdrToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCdrToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsNoBtsPath);
            FileOutputFormat.setOutputPath(job, cdrsNoBtsMobDataPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "RepbtsFilterNumComms",
                    SequenceFileInputFormat.class,
                    RepbtsFilterNumCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                repbtsAggbybtsMobDataPath, cdrsNoinfoMobDataPath,
                cdrsNoBtsMobDataPath });
            FileOutputFormat.setOutputPath(job, clientsInfoFilteredPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(cdrsNoinfoMobDataPath, true);
        fs.delete(cdrsNoBtsMobDataPath, true);

        Path clientsInfoFilteredMobDataPath = new Path(tmpDirPath,
                "clients_info_filtered_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertIntToMobData",
                    SequenceFileInputFormat.class,
                    ConvertIntToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoFilteredPath);
            FileOutputFormat.setOutputPath(job, clientsInfoFilteredMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path clientsInfoBtsPercPath = new Path(tmpDirPath,
                                               "clients_info_bts_perc");
        {
            ReduceJob job = ReduceJob.create(conf, "RepbtsJoinDistComms",
                    SequenceFileInputFormat.class,
                    RepbtsJoinDistCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                repbtsAggbybtsMobDataPath, clientsInfoFilteredMobDataPath });
            FileOutputFormat.setOutputPath(job, clientsInfoBtsPercPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(repbtsAggbybtsMobDataPath, true);
        fs.delete(clientsInfoFilteredMobDataPath, true);

        {
            ReduceJob job = ReduceJob.create(conf, "RepbtsGetRepresentativeBts",
                    SequenceFileInputFormat.class,
                    RepbtsGetRepresentativeBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsInfoBtsPercPath);
            FileOutputFormat.setOutputPath(job, clientsRepbtsPath);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path clientsRepbtsTextPath = new Path(tmpDirPath,
                                                  "clients_repbts_text");
            {
                ReduceJob job = ReduceJob.create(conf,
                        "ExportBtsCounterToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportBtsCounterToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, clientsRepbtsPath);
                FileOutputFormat.setOutputPath(job, clientsRepbtsTextPath);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(clientsBtsPath, true);
            fs.delete(clientsBtscounterPath, true);
            fs.delete(clientsInfoSpreadPath, true);
            fs.delete(clientsInfoAggbybtsPath, true);
            fs.delete(clientsInfoBtsPercPath, true);
            fs.delete(cdrsNoinfoPath, true);
            fs.delete(cdrsNoBtsPath, true);
        }
    }
}
