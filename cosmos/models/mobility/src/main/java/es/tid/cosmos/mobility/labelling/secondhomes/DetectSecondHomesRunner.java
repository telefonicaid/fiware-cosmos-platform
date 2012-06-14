package es.tid.cosmos.mobility.labelling.secondhomes;

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
import es.tid.cosmos.mobility.util.ExportPoiToTextByTwoIntReducer;
import es.tid.cosmos.mobility.util.SetMobDataInputIdByTwoIntReducer;

/**
 *
 * @author dmicol
 */
public final class DetectSecondHomesRunner {
    private DetectSecondHomesRunner() {
    }
    
    public static void run(Path cellsMobPath, Path pointsOfInterestTemp4Path,
                           Path viClientFuseAccPath, Path pairbtsAdjPath,
                           Path pointsOfInterestPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws IOException, InterruptedException, ClassNotFoundException {
        FileSystem fs = FileSystem.get(conf);
        
        Path btsMobPath = new Path(tmpDirPath, "bts_mob");
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "PoiCellToBts",
                    SequenceFileInputFormat.class,
                    PoiCellToBtsMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsMobPath);
            FileOutputFormat.setOutputPath(job, btsMobPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiPosPath = new Path(tmpDirPath, "sech_poi_pos");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PoiJoinPoisBtscoordToPoiPos",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisBtscoordToPoiPosReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTemp4Path, btsMobPath });
            FileOutputFormat.setOutputPath(job, sechPoiPosPath);
            job.waitForCompletion(true);
        }

        Path nodbtsPoiPath = new Path(tmpDirPath, "nodbts_poi");
        {
            CosmosJob job = CosmosJob.createMapJob(conf, "PoiJoinPoisBtscoordToPoi",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisBtscoordToPoiMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
            FileOutputFormat.setOutputPath(job, nodbtsPoiPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiInoutPath = new Path(tmpDirPath, "vector_bts");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PoiJoinPoisViToPoiPos",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisViToPoiPosReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPoiPosPath, viClientFuseAccPath });
            FileOutputFormat.setOutputPath(job, sechPoiInoutPath);
            job.waitForCompletion(true);
        }

        Path nodbtsInoutPath = new Path(tmpDirPath, "nodbts_inout");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PoiJoinPoisViToTwoInt",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisViToTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPoiPosPath, viClientFuseAccPath });
            FileOutputFormat.setOutputPath(job, nodbtsInoutPath);
            job.waitForCompletion(true);
        }

        Path sechPotSecHomePath = new Path(tmpDirPath, "sech_pot_sec_home");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "GetPairsSechomePois",
                    SequenceFileInputFormat.class,
                    GetPairsSechomePoisReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, sechPoiInoutPath);
            FileOutputFormat.setOutputPath(job, sechPotSecHomePath);
            job.waitForCompletion(true);
        }
        
        Path sechPotSecHomeInputIdPath = new Path(tmpDirPath, "sech_pot_sec_home_id");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "GetPairsSechomePois",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 0);
            FileInputFormat.setInputPaths(job, sechPotSecHomePath);
            FileOutputFormat.setOutputPath(job, sechPotSecHomeInputIdPath);
            job.waitForCompletion(true);
        }
        
        Path pairbtsAdjInputIdPath = new Path(tmpDirPath, "pairbts_adj_id");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "GetPairsSechomePois",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 1);
            FileInputFormat.setInputPaths(job, pairbtsAdjPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjInputIdPath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomePath = new Path(tmpDirPath, "nodbts_sechome");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf,
                    "PoiFilterSechomeAdjacent",
                    SequenceFileInputFormat.class,
                    PoiFilterSechomeAdjacentReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPotSecHomeInputIdPath, pairbtsAdjInputIdPath });
            FileOutputFormat.setOutputPath(job, nodbtsSechomePath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomeUniqPath = new Path(tmpDirPath,
                                              "nodbts_sechome_uniq");
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PoiDeleteSechomeDuplicate",
                    SequenceFileInputFormat.class,
                    PoiDeleteSechomeDuplicateReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsSechomePath);
            FileOutputFormat.setOutputPath(job, nodbtsSechomeUniqPath);
            job.waitForCompletion(true);
        }
        
        {
            CosmosJob job = CosmosJob.createReduceJob(conf, "PoiJoinSechomeResults",
                    SequenceFileInputFormat.class,
                    PoiJoinSechomeResultsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                nodbtsPoiPath,
                nodbtsInoutPath,
                nodbtsSechomeUniqPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestPath);
            job.waitForCompletion(true);
        }
        
        if (isDebug) {
            Path pointsOfInterestTextPath = new Path(tmpDirPath,
                                                     "points_of_interest_text");
            {
                CosmosJob job = CosmosJob.createReduceJob(conf,
                        "ExportPoiToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestPath);
                FileOutputFormat.setOutputPath(job, pointsOfInterestTextPath);
                job.waitForCompletion(true);
            }
        } else {
            fs.delete(btsMobPath, true);
            fs.delete(nodbtsPoiPath, true);
            fs.delete(sechPoiPosPath, true);
            fs.delete(sechPoiInoutPath, true);
            fs.delete(nodbtsInoutPath, true);
            fs.delete(sechPotSecHomePath, true);
            fs.delete(nodbtsSechomePath, true);
            fs.delete(nodbtsSechomeUniqPath, true);
            fs.delete(pointsOfInterestTemp4Path, true);
        }
    }
}
