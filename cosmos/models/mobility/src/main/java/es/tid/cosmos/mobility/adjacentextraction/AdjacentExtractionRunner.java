package es.tid.cosmos.mobility.adjacentextraction;

import java.util.EnumSet;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.base.mapreduce.CleanupOptions;
import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.util.*;

/**
 *
 * @author dmicol
 */
public final class AdjacentExtractionRunner {
    private AdjacentExtractionRunner() {
    }
    
    public static void run(Path pointsOfInterestPath, Path pairbtsAdjPath,
                           Path pointsOfInterestIdPath, Path tmpDirPath,
                           boolean isDebug, Configuration conf)
            throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        Path poisIdPath = new Path(tmpDirPath, "pois_id");
        {
            MapJob job = MapJob.create(conf, "AdjAddUniqueIdPoiToPoiNew",
                    SequenceFileInputFormat.class,
                    AdjAddUniqueIdPoiToPoiNewMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestPath);
            FileOutputFormat.setOutputPath(job, poisIdPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path poisTablePath = new Path(tmpDirPath, "pois_table");
        {
            MapJob job = MapJob.create(conf, "AdjAddUniqueIdPoiToTwoInt",
                    SequenceFileInputFormat.class,
                    AdjAddUniqueIdPoiToTwoIntMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestPath);
            FileOutputFormat.setOutputPath(job, poisTablePath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path poiPairbtsPath = new Path(tmpDirPath, "poi_pairbts");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjGroupTypePoiClient",
                    SequenceFileInputFormat.class,
                    AdjGroupTypePoiClientReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisIdPath);
            FileOutputFormat.setOutputPath(job, poiPairbtsPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path poiPairbtsMobDataPath = new Path(tmpDirPath,
                                              "poi_pairbts_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertTwoIntToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertTwoIntToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poiPairbtsPath);
            FileOutputFormat.setOutputPath(job, poiPairbtsMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path pairbtsAdjMobDataPath = new Path(tmpDirPath,
                                              "pairbts_adj_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertNullToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertNullToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pairbtsAdjPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path poiPairbtsMobDataWithInputIdPath = new Path(tmpDirPath,
                "poi_pairbts_mob_data_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 0);
            FileInputFormat.setInputPaths(job, poiPairbtsMobDataPath);
            FileOutputFormat.setOutputPath(job,
                                           poiPairbtsMobDataWithInputIdPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path pairbtsAdjMobDataWithInputIdPath = new Path(tmpDirPath,
                "pairbts_adj_mob_data_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 1);
            FileInputFormat.setInputPaths(job, pairbtsAdjMobDataPath);
            FileOutputFormat.setOutputPath(job,
                                           pairbtsAdjMobDataWithInputIdPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path poiPairbtsAdjPath = new Path(tmpDirPath, "poi_pairbts_adj");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjJoinPairbtsAdjbts",
                    SequenceFileInputFormat.class,
                    AdjJoinPairbtsAdjbtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                poiPairbtsMobDataWithInputIdPath,
                pairbtsAdjMobDataWithInputIdPath });
            FileOutputFormat.setOutputPath(job, poiPairbtsAdjPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        long numIndicesLeft;
        do {
            Path poiPairbtsIndexPath = new Path(tmpDirPath,
                                                "poi_pairbts_index");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjPutMaxId",
                        SequenceFileInputFormat.class,
                        AdjPutMaxIdReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job, poiPairbtsIndexPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }

            Path poiPairbtsIndexMobDataPath = new Path(tmpDirPath,
                    "poi_pairbts_index_mob_data");
            {
                ReduceJob job = ReduceJob.create(conf, "ConvertTwoIntToMobData",
                        SequenceFileInputFormat.class,
                        ConvertTwoIntToMobDataReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsIndexPath);
                FileOutputFormat.setOutputPath(job, poiPairbtsIndexMobDataPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }

            Path poisTableMobDataPath = new Path(tmpDirPath,
                                                 "pois_table_mob_data");
            {
                ReduceJob job = ReduceJob.create(conf, "ConvertTwoIntToMobData",
                        SequenceFileInputFormat.class,
                        ConvertTwoIntToMobDataReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poisTablePath);
                FileOutputFormat.setOutputPath(job, poisTableMobDataPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            
            Path poisTableTmpPath = new Path(tmpDirPath, "pois_table_tmp");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjUpdatePoisTable",
                        SequenceFileInputFormat.class,
                        AdjUpdatePoisTableReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poisTableMobDataPath, poiPairbtsIndexMobDataPath });
                FileOutputFormat.setOutputPath(job, poisTableTmpPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            
            fs.delete(poisTablePath, true);
            fs.delete(poisTableMobDataPath, true);
            fs.rename(poisTableTmpPath, poisTablePath);
            
            Path poiPairbtsAdjMobDataPath = new Path(tmpDirPath,
                                                     "poi_pairbts_adj_mob_data");
            {
                ReduceJob job = ReduceJob.create(conf, "ConvertTwoIntToMobData",
                        SequenceFileInputFormat.class,
                        ConvertTwoIntToMobDataReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job, poiPairbtsAdjMobDataPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            
            Path poiPairbtsCh1Path = new Path(tmpDirPath, "poi_pairbts_ch1");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSwapPoiIdSt1",
                        SequenceFileInputFormat.class,
                        AdjSwapPoiIdSt1Reducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poiPairbtsAdjMobDataPath, poiPairbtsIndexMobDataPath });
                FileOutputFormat.setOutputPath(job, poiPairbtsCh1Path);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }

            fs.delete(poiPairbtsAdjPath, true);
            fs.delete(poiPairbtsAdjMobDataPath, true);
            
            Path poiPairbtsCh1MobDataPath = new Path(tmpDirPath,
                                                     "poi_pairbts_ch1_mob_data");
            {
                ReduceJob job = ReduceJob.create(conf, "ConvertTwoIntToMobData",
                        SequenceFileInputFormat.class,
                        ConvertTwoIntToMobDataReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsCh1Path);
                FileOutputFormat.setOutputPath(job, poiPairbtsCh1MobDataPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSwapPoiIdSt2",
                        SequenceFileInputFormat.class,
                        AdjSwapPoiIdSt2Reducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poiPairbtsCh1MobDataPath, poiPairbtsIndexMobDataPath });
                FileOutputFormat.setOutputPath(job, poiPairbtsAdjPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            
            Path nindSpreadPath = new Path(tmpDirPath, "nind_spread");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSpreadCount",
                        SequenceFileInputFormat.class,
                        AdjSpreadCountReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job, nindSpreadPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }

            Path numIndexPath = new Path(tmpDirPath, "num_index");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjCountIndexes",
                        SequenceFileInputFormat.class,
                        AdjCountIndexesReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, nindSpreadPath);
                FileOutputFormat.setOutputPath(job, numIndexPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
            numIndicesLeft = conf.getLong(
                    AdjCountIndexesReducer.NUM_INDICES_LEFT_TAG, 0);
        } while (numIndicesLeft > 0);
        
        Path poiPoimodPath = new Path(tmpDirPath, "poi_poimod");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjSpreadTableByPoiId",
                    SequenceFileInputFormat.class,
                    AdjSpreadTableByPoiIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisTablePath);
            FileOutputFormat.setOutputPath(job, poiPoimodPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path poiIdPoiPath = new Path(tmpDirPath, "poiId_poi");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjSpreadPoisByPoiId",
                    SequenceFileInputFormat.class,
                    AdjSpreadPoisByPoiIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisIdPath);
            FileOutputFormat.setOutputPath(job, poiIdPoiPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        Path poiPoimodMobDataPath = new Path(tmpDirPath, "poi_poimod_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertLongToMobData",
                    SequenceFileInputFormat.class,
                    ConvertLongToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poiPoimodPath);
            FileOutputFormat.setOutputPath(job, poiPoimodMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path poiIdPoiMobDataPath = new Path(tmpDirPath, "poiId_poi_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertPoiNewToMobData",
                    SequenceFileInputFormat.class,
                    ConvertPoiNewToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poiIdPoiPath);
            FileOutputFormat.setOutputPath(job, poiIdPoiMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path pointsOfInterestModPath = new Path(tmpDirPath,
                                                "points_of_interest_mod");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjJoinNewPoiId",
                    SequenceFileInputFormat.class,
                    AdjJoinNewPoiIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                poiPoimodMobDataPath, poiIdPoiMobDataPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestModPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
      
        Path pointsOfInterestMobDataPath = new Path(tmpDirPath,
                "points_of_interest_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertPoiToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertPoiToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        Path pointsOfInterestModMobDataPath = new Path(tmpDirPath,
                "points_of_interest_mod_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf,
                    "ConvertPoiNewToMobDataByTwoInt",
                    SequenceFileInputFormat.class,
                    ConvertPoiNewToMobDataByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestModPath);
            FileOutputFormat.setOutputPath(job, pointsOfInterestModMobDataPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "AdjChangePoisId",
                    SequenceFileInputFormat.class,
                    AdjChangePoisIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestMobDataPath, pointsOfInterestModMobDataPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestIdPath);
            job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        if (isDebug) {
            Path pointsOfInterestIdTextPath = new Path(tmpDirPath,
                    "points_of_interest_id_text");
            {
                ReduceJob job = ReduceJob.create(conf,
                        "ExportPoiToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextByTwoIntReducer.class,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestIdPath);
                FileOutputFormat.setOutputPath(job, pointsOfInterestIdTextPath);
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
            }
        }
    }
}
