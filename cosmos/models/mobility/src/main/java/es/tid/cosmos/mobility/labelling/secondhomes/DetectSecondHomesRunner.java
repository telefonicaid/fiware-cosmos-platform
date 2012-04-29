package es.tid.cosmos.mobility.labelling.secondhomes;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

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
            ReduceJob job = ReduceJob.create(conf, "PoiCellToBts",
                    SequenceFileInputFormat.class,
                    PoiCellToBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsMobPath);
            FileOutputFormat.setOutputPath(job, btsMobPath);
            job.waitForCompletion(true);
        }

        Path pointsOfInterestTemp4MobDataPath = new Path(tmpDirPath,
                "points_of_interest_temp4_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertPoiToMobData",
                    SequenceFileInputFormat.class,
                    ConvertPoiToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
            FileOutputFormat.setOutputPath(job,
                                           pointsOfInterestTemp4MobDataPath);
            job.waitForCompletion(true);
        }

        Path btsMobMobDataPath = new Path(tmpDirPath, "bts_mob_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertCellToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCellToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, btsMobPath);
            FileOutputFormat.setOutputPath(job, btsMobMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiPosPath = new Path(tmpDirPath, "sech_poi_pos");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoisBtscoordToPoiPos",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisBtscoordToPoiPosReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestTemp4MobDataPath, btsMobMobDataPath });
            FileOutputFormat.setOutputPath(job, sechPoiPosPath);
            job.waitForCompletion(true);
        }

        Path nodbtsPoiPath = new Path(tmpDirPath, "nodbts_poi");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoisBtscoordToPoi",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisBtscoordToPoiReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestTemp4Path);
            FileOutputFormat.setOutputPath(job, nodbtsPoiPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(pointsOfInterestTemp4MobDataPath, true);
        fs.delete(btsMobMobDataPath, true);

        Path sechPoiPosMobDataPath = new Path(tmpDirPath,
                                              "sech_poi_pos_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertPoiPosToMobData",
                    SequenceFileInputFormat.class,
                    ConvertPoiPosToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, sechPoiPosPath);
            FileOutputFormat.setOutputPath(job, sechPoiPosMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path viClientFuseAccMobDataPath = new Path(tmpDirPath,
                "vi_client_fuse_acc_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertMobViMobVarsToMobData",
                    SequenceFileInputFormat.class,
                    ConvertMobViMobVarsToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, viClientFuseAccPath);
            FileOutputFormat.setOutputPath(job, viClientFuseAccMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path sechPoiInoutPath = new Path(tmpDirPath, "vector_bts");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinPoisViToPoiPos",
                    SequenceFileInputFormat.class,
                    PoiJoinPoisViToPoiPosReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPoiPosMobDataPath, viClientFuseAccMobDataPath });
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
                sechPoiPosMobDataPath, viClientFuseAccMobDataPath });
            FileOutputFormat.setOutputPath(job, nodbtsInoutPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(sechPoiPosMobDataPath, true);
        fs.delete(viClientFuseAccMobDataPath, true);

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

        Path sechPotSecHomeMobDataPath = new Path(tmpDirPath,
                                                  "sech_pot_sec_home_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertLongToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertLongToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, sechPotSecHomePath);
            FileOutputFormat.setOutputPath(job, sechPotSecHomeMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path pairsbtsAdjMobDataPath = new Path(tmpDirPath,
                                               "pairbts_adj_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertNullToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertNullToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pairbtsAdjPath);
            FileOutputFormat.setOutputPath(job, pairsbtsAdjMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomePath = new Path(tmpDirPath, "nodbts_sechome");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiFilterSechomeAdjacent",
                    SequenceFileInputFormat.class,
                    PoiFilterSechomeAdjacentReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                sechPotSecHomeMobDataPath, pairsbtsAdjMobDataPath });
            FileOutputFormat.setOutputPath(job, nodbtsSechomePath);
            job.waitForCompletion(true);
        }
        
        fs.delete(sechPotSecHomeMobDataPath, true);
        fs.delete(pairsbtsAdjMobDataPath, true);
        
        Path nodbtsSechomeUniqPath = new Path(tmpDirPath, "nodbts_sechome_uniq");
        {
            ReduceJob job = ReduceJob.create(conf, "PoiDeleteSechomeDuplicate",
                    SequenceFileInputFormat.class,
                    PoiDeleteSechomeDuplicateReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsSechomePath);
            FileOutputFormat.setOutputPath(job, nodbtsSechomeUniqPath);
            job.waitForCompletion(true);
        }

        Path nodbtsPoiMobDataPath = new Path(tmpDirPath, "nodbts_poi_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertPoiToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertPoiToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsPoiPath);
            FileOutputFormat.setOutputPath(job, nodbtsPoiMobDataPath);
            job.waitForCompletion(true);
        }

        Path nodbtsInoutMobDataPath = new Path(tmpDirPath,
                                               "nodbts_inout_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertTwoIntToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertTwoIntToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsInoutPath);
            FileOutputFormat.setOutputPath(job, nodbtsInoutMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path nodbtsSechomeUniqMobDataPath = new Path(tmpDirPath,
                "nodbts_sechome_uniq_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertNullToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertNullToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, nodbtsSechomeUniqPath);
            FileOutputFormat.setOutputPath(job, nodbtsSechomeUniqMobDataPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "PoiJoinSechomeResults",
                    SequenceFileInputFormat.class,
                    PoiJoinSechomeResultsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                nodbtsPoiMobDataPath,
                nodbtsInoutMobDataPath,
                nodbtsSechomeUniqMobDataPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(nodbtsPoiMobDataPath, true);
        fs.delete(nodbtsInoutMobDataPath, true);
        fs.delete(nodbtsSechomeUniqMobDataPath, true);
        
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
