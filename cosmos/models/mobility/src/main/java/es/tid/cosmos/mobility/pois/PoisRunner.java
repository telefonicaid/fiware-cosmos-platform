package es.tid.cosmos.mobility.pois;

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
import es.tid.cosmos.mobility.util.ExportBtsCounterToTextByTwoIntReducer;

/**
 *
 * @author dmicol
 */
public final class PoisRunner {
    private PoisRunner() {
    }
    
    public static CosmosWorkflow run(Path tmpDirPath, Path clientsBtsPath,
            Path clientsInfoPath, Path cdrsNoinfoPath, Path cdrsNoBtsPath,
            Path clientsInfoFilteredPath, Path clientsRepbtsPath,
            boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        
        Path clientsBtscounterPath = new Path(tmpDirPath, "clients_btscounter");
        CosmosJob clientsBtscounterJob = CosmosJob.createReduceJob(
                conf, "NodeBtsCounter",
                SequenceFileInputFormat.class,
                NodeBtsCounterReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsBtscounterJob, clientsBtsPath);
        FileOutputFormat.setOutputPath(clientsBtscounterJob,
                                        clientsBtscounterPath);
        clientsBtscounterJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(clientsBtscounterJob);

        CosmosJob clientsInfoJob = CosmosJob.createReduceJob(conf, "NodeMobInfo",
                SequenceFileInputFormat.class,
                NodeMobInfoReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsInfoJob, clientsBtscounterPath);
        FileOutputFormat.setOutputPath(clientsInfoJob, clientsInfoPath);
        clientsInfoJob.addDependentWorkflow(clientsBtscounterJob);
        wfList.add(clientsInfoJob);

        Path clientsInfoSpreadPath = new Path(tmpDirPath,
                                              "clients_info_spread");
        CosmosJob clientsInfoSpreadJob = CosmosJob.createMapJob(conf,
                "RepbtsSpreadNodebts",
                SequenceFileInputFormat.class,
                RepbtsSpreadNodebtsMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsInfoSpreadJob, clientsInfoPath);
        FileOutputFormat.setOutputPath(clientsInfoSpreadJob,
                                        clientsInfoSpreadPath);
        clientsInfoSpreadJob.setDeleteOutputOnExit(!isDebug);
        clientsInfoSpreadJob.addDependentWorkflow(clientsInfoJob);
        wfList.add(clientsInfoSpreadJob);

        Path clientsInfoAggbybtsPath = new Path(tmpDirPath,
                                                "clients_info_aggbybts");
        CosmosJob clientsInfoAggbybtsJob = CosmosJob.createReduceJob(conf,
                "RepbtsAggbybts",
                SequenceFileInputFormat.class,
                RepbtsAggbybtsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsInfoAggbybtsJob,
                                        clientsInfoSpreadPath);
        FileOutputFormat.setOutputPath(clientsInfoAggbybtsJob,
                                        clientsInfoAggbybtsPath);
        clientsInfoAggbybtsJob.setDeleteOutputOnExit(!isDebug);
        clientsInfoAggbybtsJob.addDependentWorkflow(clientsInfoSpreadJob);
        wfList.add(clientsInfoAggbybtsJob);
        
        CosmosJob clientsInfoFilteredJob = CosmosJob.createReduceJob(
                conf, "RepbtsFilterNumComms",
                SequenceFileInputFormat.class,
                RepbtsFilterNumCommsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsInfoFilteredJob, new Path[] {
            clientsInfoAggbybtsPath, cdrsNoinfoPath, cdrsNoBtsPath });
        FileOutputFormat.setOutputPath(clientsInfoFilteredJob,
                                       clientsInfoFilteredPath);
        clientsInfoFilteredJob.setDeleteOutputOnExit(!isDebug);
        clientsInfoFilteredJob.addDependentWorkflow(clientsInfoAggbybtsJob);
        wfList.add(clientsInfoFilteredJob);
        
        Path clientsInfoBtsPercPath = new Path(tmpDirPath,
                                               "clients_info_bts_perc");
        CosmosJob clientsInfoBtsPercJob = CosmosJob.createReduceJob(conf,
                "RepbtsJoinDistComms",
                SequenceFileInputFormat.class,
                RepbtsJoinDistCommsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsInfoBtsPercJob,
                clientsInfoAggbybtsPath, clientsInfoFilteredPath);
        FileOutputFormat.setOutputPath(clientsInfoBtsPercJob,
                                       clientsInfoBtsPercPath);
        clientsInfoBtsPercJob.setDeleteOutputOnExit(!isDebug);
        clientsInfoBtsPercJob.addDependentWorkflow(clientsInfoAggbybtsJob);
        clientsInfoBtsPercJob.addDependentWorkflow(clientsInfoFilteredJob);
        wfList.add(clientsInfoBtsPercJob);

        CosmosJob clientsRepbtsJob = CosmosJob.createMapJob(conf,
                "RepbtsGetRepresentativeBts",
                SequenceFileInputFormat.class,
                RepbtsGetRepresentativeBtsMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientsRepbtsJob, clientsInfoBtsPercPath);
        FileOutputFormat.setOutputPath(clientsRepbtsJob, clientsRepbtsPath);
        clientsRepbtsJob.addDependentWorkflow(clientsInfoBtsPercJob);
        wfList.add(clientsRepbtsJob);

        if (isDebug) {
            Path clientsRepbtsTextPath = new Path(tmpDirPath,
                                                  "clients_repbts_text");
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ExportBtsCounterToTextByTwoInt",
                    SequenceFileInputFormat.class,
                    ExportBtsCounterToTextByTwoIntReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, clientsRepbtsPath);
            FileOutputFormat.setOutputPath(job, clientsRepbtsTextPath);
            job.addDependentWorkflow(clientsRepbtsJob);
            wfList.add(job);
        }
        return wfList;
    }
}
