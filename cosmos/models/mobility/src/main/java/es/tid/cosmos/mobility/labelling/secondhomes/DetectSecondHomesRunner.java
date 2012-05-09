package es.tid.cosmos.mobility.labelling.secondhomes;

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
public final class DetectSecondHomesRunner {
    private DetectSecondHomesRunner() {
    }
    
    public static void run(Path cellsMobPath, Path pointsOfInterestTemp4Path,
                           Path viClientFuseAccPath, Path pairbtsAdjPath,
                           Path pointsOfInterestPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path btsMobPath = new Path(tmpDirPath, "bts_mob");
        {
            MapJob job = MapJob.create(conf, "PoiCellToBts",
                    SequenceFileInputFormat.class,
                    PoiCellToBtsMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsMobPath);
            FileOutputFormat.setOutputPath(job, btsMobPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiPosPath = new Path(tmpDirPath, "sech_poi_pos");
        {
            ReduceJob job = ReduceJob.create(conf,
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
            MapJob job = MapJob.create(conf, "PoiJoinPoisBtscoordToPoi",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisBtscoordToPoiMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
            FileOutputFormat.setOutputPath(job, nodbtsPoiPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiInoutPath = new Path(tmpDirPath, "vector_bts");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoisViToPoiPos",
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
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoisViToTwoInt",
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
            ReduceJob job = ReduceJob.create(conf, "GetPairsSechomePois",
                    SequenceFileInputFormat.class,
                    GetPairsSechomePoisReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, sechPoiInoutPath);
            FileOutputFormat.setOutputPath(job, sechPotSecHomePath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomePath = new Path(tmpDirPath, "nodbts_sechome");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "PoiFilterSechomeAdjacent",
                    SequenceFileInputFormat.class,
                    PoiFilterSechomeAdjacentReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPotSecHomePath, pairbtsAdjPath });
            FileOutputFormat.setOutputPath(job, nodbtsSechomePath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomeUniqPath = new Path(tmpDirPath,
                                              "nodbts_sechome_uniq");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiDeleteSechomeDuplicate",
                    SequenceFileInputFormat.class,
                    PoiDeleteSechomeDuplicateReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsSechomePath);
            FileOutputFormat.setOutputPath(job, nodbtsSechomeUniqPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinSechomeResults",
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
                ReduceJob job = ReduceJob.create(conf,
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
