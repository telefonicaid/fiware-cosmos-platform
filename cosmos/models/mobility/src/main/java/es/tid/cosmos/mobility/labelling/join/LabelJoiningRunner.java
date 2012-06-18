package es.tid.cosmos.mobility.labelling.join;

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
import es.tid.cosmos.mobility.util.ExportPoiToTextReducer;

/**
 *
 * @author dmicol
 */
public class LabelJoiningRunner {
    public LabelJoiningRunner() {
    }
    
    public CosmosWorkflow run(Path pointsOfInterestTempPath,
            Path vectorClientClusterPath, Path vectorClientbtsClusterPath,
            Path vectorBtsClusterPath, Path pointsOfInterestTemp4Path,
            Path tmpDirPath, boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        
        Path pointsOfInterestTemp2Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp2");
        CosmosJob pointsOfInterestTemp2Job = CosmosJob.createReduceJob(conf,
                "ClusterAggNodeClusterByNodbts",
                SequenceFileInputFormat.class,
                ClusterAggNodeClusterByNodbtsReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(pointsOfInterestTemp2Job, new Path[] {
            pointsOfInterestTempPath, vectorClientClusterPath });
        FileOutputFormat.setOutputPath(pointsOfInterestTemp2Job,
                                       pointsOfInterestTemp2Path);
        pointsOfInterestTemp2Job.setDeleteOutputOnExit(!isDebug);
        wfList.add(pointsOfInterestTemp2Job);

        Path potpoiPath = new Path(tmpDirPath, "potpoi");
        CosmosJob potpoiJob = CosmosJob.createReduceJob(conf,
                "ClusterAggNodeClusterByNodlbl",
                SequenceFileInputFormat.class,
                ClusterAggNodeClusterByNodlblReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(potpoiJob, new Path[] {
            pointsOfInterestTempPath, vectorClientClusterPath });
        FileOutputFormat.setOutputPath(potpoiJob, potpoiPath);
        potpoiJob.setDeleteOutputOnExit(!isDebug);
        wfList.add(potpoiJob);
        
        Path clientbtsNodpoilblPath = new Path(tmpDirPath,
                                               "clientbts_nodpoilbl");
        CosmosJob clientbtsNodpoilblJob = CosmosJob.createReduceJob(conf,
                "ClusterSpreadNodelblPoilbl",
                SequenceFileInputFormat.class,
                ClusterSpreadNodelblPoilblReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientbtsNodpoilblJob,
                                      pointsOfInterestTemp2Path);
        FileOutputFormat.setOutputPath(clientbtsNodpoilblJob,
                                       clientbtsNodpoilblPath);
        clientbtsNodpoilblJob.setDeleteOutputOnExit(!isDebug);
        clientbtsNodpoilblJob.addDependentWorkflow(pointsOfInterestTemp2Job);
        wfList.add(clientbtsNodpoilblJob);
        
        Path clientbtsNodpoiCountPath = new Path(tmpDirPath,
                                                 "clientbts_nodpoi_count");
        CosmosJob clientbtsNodpoiCountJob = CosmosJob.createReduceJob(conf,
                "ClusterCountMajPoiByNode",
                SequenceFileInputFormat.class,
                ClusterCountMajPoiByNodeReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientbtsNodpoiCountJob,
                                      clientbtsNodpoilblPath);
        FileOutputFormat.setOutputPath(clientbtsNodpoiCountJob,
                                       clientbtsNodpoiCountPath);
        clientbtsNodpoiCountJob.setDeleteOutputOnExit(!isDebug);
        clientbtsNodpoiCountJob.addDependentWorkflow(clientbtsNodpoilblJob);
        wfList.add(clientbtsNodpoiCountJob);
        
        Path clientbtsNodPoimajPath = new Path(tmpDirPath,
                                               "clientbts_nod_poimaj");
        CosmosJob clientbtsNodPoimajJob = CosmosJob.createReduceJob(conf,
                "ClusterGetMajPoiByNode",
                SequenceFileInputFormat.class,
                ClusterGetMajPoiByNodeReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(clientbtsNodPoimajJob,
                                      clientbtsNodpoiCountPath);
        FileOutputFormat.setOutputPath(clientbtsNodPoimajJob,
                                       clientbtsNodPoimajPath);
        clientbtsNodPoimajJob.setDeleteOutputOnExit(!isDebug);
        clientbtsNodPoimajJob.addDependentWorkflow(clientbtsNodpoiCountJob);
        wfList.add(clientbtsNodPoimajJob);

        Path poisLabeledPath = new Path(tmpDirPath, "pois_labeled");
        CosmosJob poisLabeledJob = CosmosJob.createReduceJob(conf,
                "ClusterJoinPotPoiLabel",
                SequenceFileInputFormat.class,
                ClusterJoinPotPoiLabelReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(poisLabeledJob, new Path[] { potpoiPath,
                clientbtsNodPoimajPath });
        FileOutputFormat.setOutputPath(poisLabeledJob, poisLabeledPath);
        poisLabeledJob.setDeleteOutputOnExit(!isDebug);
        poisLabeledJob.addDependentWorkflow(potpoiJob);
        poisLabeledJob.addDependentWorkflow(clientbtsNodPoimajJob);
        wfList.add(poisLabeledJob);
        
        Path pointsOfInterestTemp3Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp3");
        CosmosJob pointsOfInterestTemp3Job = CosmosJob.createReduceJob(conf,
                "ClusterAggPotPoiPoisToPoi",
                SequenceFileInputFormat.class,
                ClusterAggPotPoiPoisToPoiReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(pointsOfInterestTemp3Job, new Path[] {
                pointsOfInterestTemp2Path, poisLabeledPath });
        FileOutputFormat.setOutputPath(pointsOfInterestTemp3Job,
                                       pointsOfInterestTemp3Path);
        pointsOfInterestTemp3Job.setDeleteOutputOnExit(!isDebug);
        pointsOfInterestTemp3Job.addDependentWorkflow(pointsOfInterestTemp2Job);
        pointsOfInterestTemp3Job.addDependentWorkflow(poisLabeledJob);
        wfList.add(pointsOfInterestTemp3Job);
        
        Path vectorClientbtsClusterAddPath = new Path(tmpDirPath,
                "vector_clientbts_cluster_add");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "ClusterAggPotPoiPoisToCluster",
                    SequenceFileInputFormat.class,
                    ClusterAggPotPoiPoisToClusterReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                vectorClientbtsClusterPath, poisLabeledPath });
            FileOutputFormat.setOutputPath(job, vectorClientbtsClusterAddPath);
            job.setDeleteOutputOnExit(!isDebug);
            job.addDependentWorkflow(poisLabeledJob);
            wfList.add(job);
        }
        
        CosmosJob pointsOfInterestTemp4Job = CosmosJob.createReduceJob(conf,
                "ClusterAggBtsCluster",
                SequenceFileInputFormat.class,
                ClusterAggBtsClusterReducer.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(pointsOfInterestTemp4Job, new Path[] {
            pointsOfInterestTemp3Path, vectorBtsClusterPath });
        FileOutputFormat.setOutputPath(pointsOfInterestTemp4Job, pointsOfInterestTemp4Path);
        pointsOfInterestTemp4Job.setDeleteOutputOnExit(!isDebug);
        pointsOfInterestTemp4Job.addDependentWorkflow(pointsOfInterestTemp3Job);
        wfList.add(pointsOfInterestTemp4Job);
        
        if (isDebug) {
            Path pointsOfInterestTemp4TextPath = new Path(tmpDirPath,
                    "points_of_interest_temp4_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf, "ExportPoiToText",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
                FileOutputFormat.setOutputPath(job,
                                               pointsOfInterestTemp4TextPath);
                job.addDependentWorkflow(pointsOfInterestTemp4Job);
                wfList.add(job);
            }
        }
        return wfList;
    }
}
