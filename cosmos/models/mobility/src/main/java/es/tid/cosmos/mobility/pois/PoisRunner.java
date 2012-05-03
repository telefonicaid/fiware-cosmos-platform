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

        Path clientsInfoSpreadPath = new Path(tmpDirPath,
                                              "clients_info_spread");
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
        
        {
            ReduceJob job = ReduceJob.create(conf, "RepbtsFilterNumComms",
                    SequenceFileInputFormat.class,
                    RepbtsFilterNumCommsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                clientsInfoAggbybtsPath, cdrsNoinfoPath, cdrsNoBtsPath });
            FileOutputFormat.setOutputPath(job, clientsInfoFilteredPath);
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
                clientsInfoAggbybtsPath, clientsInfoFilteredPath });
            FileOutputFormat.setOutputPath(job, clientsInfoBtsPercPath);
            job.waitForCompletion(true);
        }

        {
            MapJob job = MapJob.create(conf, "RepbtsGetRepresentativeBts",
                    SequenceFileInputFormat.class,
                    RepbtsGetRepresentativeBtsMapper.class,
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
