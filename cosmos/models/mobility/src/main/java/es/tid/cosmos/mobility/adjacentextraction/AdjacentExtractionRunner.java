package es.tid.cosmos.mobility.adjacentextraction;

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
            job.waitForCompletion(true);
        }

        Path poisTablePath = new Path(tmpDirPath, "pois_table");
        {
            MapJob job = MapJob.create(conf, "AdjAddUniqueIdPoiToTwoInt",
                    SequenceFileInputFormat.class,
                    AdjAddUniqueIdPoiToTwoIntMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, pointsOfInterestPath);
            FileOutputFormat.setOutputPath(job, poisTablePath);
            job.waitForCompletion(true);
        }

        Path poiPairbtsPath = new Path(tmpDirPath, "poi_pairbts");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjGroupTypePoiClient",
                    SequenceFileInputFormat.class,
                    AdjGroupTypePoiClientReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisIdPath);
            FileOutputFormat.setOutputPath(job, poiPairbtsPath);
            job.waitForCompletion(true);
        }
        
        Path poiPairbtsWithInputIdPath = new Path(tmpDirPath,
                                                  "poi_pairbts_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 0);
            FileInputFormat.setInputPaths(job, poiPairbtsPath);
            FileOutputFormat.setOutputPath(job, poiPairbtsWithInputIdPath);
            job.waitForCompletion(true);
        }
        
        Path pairbtsAdjWithInputIdPath = new Path(tmpDirPath,
                                                  "pairbts_adj_with_input_id");
        {
            ReduceJob job = ReduceJob.create(conf, "SetMobDataInputIdByTwoInt",
                    SequenceFileInputFormat.class,
                    SetMobDataInputIdByTwoIntReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setInt("input_id", 1);
            FileInputFormat.setInputPaths(job, pairbtsAdjPath);
            FileOutputFormat.setOutputPath(job, pairbtsAdjWithInputIdPath);
            job.waitForCompletion(true);
        }
        
        Path poiPairbtsAdjPath = new Path(tmpDirPath, "poi_pairbts_adj");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjJoinPairbtsAdjbts",
                    SequenceFileInputFormat.class,
                    AdjJoinPairbtsAdjbtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                poiPairbtsWithInputIdPath, pairbtsAdjWithInputIdPath });
            FileOutputFormat.setOutputPath(job, poiPairbtsAdjPath);
            job.waitForCompletion(true);
        }
        
        long numIndicesLeft = 1L;
        while (numIndicesLeft > 0L) {
            Path poiPairbtsIndexPath = new Path(tmpDirPath,
                                                "poi_pairbts_index");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjPutMaxId",
                        SequenceFileInputFormat.class,
                        AdjPutMaxIdReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job, poiPairbtsIndexPath);
                job.waitForCompletion(true);
            }

            Path poisTableWithInputIdPath = new Path(tmpDirPath,
                    "pois_table_with_input_id");
            {
                ReduceJob job = ReduceJob.create(conf, "SetMobDataInputId",
                        SequenceFileInputFormat.class,
                        SetMobDataInputIdReducer.class,
                        SequenceFileOutputFormat.class);
                job.getConfiguration().setInt("input_id", 0);
                FileInputFormat.setInputPaths(job, poisTablePath);
                FileOutputFormat.setOutputPath(job, poisTableWithInputIdPath);
                job.waitForCompletion(true);
            }
            
            Path poiPairbtsIndexWithInputIdPath = new Path(tmpDirPath,
                    "poi_pairbts_index_with_input_id");
            {
                ReduceJob job = ReduceJob.create(conf, "SetMobDataInputId",
                        SequenceFileInputFormat.class,
                        SetMobDataInputIdReducer.class,
                        SequenceFileOutputFormat.class);
                job.getConfiguration().setInt("input_id", 1);
                FileInputFormat.setInputPaths(job, poiPairbtsIndexPath);
                FileOutputFormat.setOutputPath(job,
                        poiPairbtsIndexWithInputIdPath);
                job.waitForCompletion(true);
            }
            
            Path poisTableTmpPath = new Path(tmpDirPath, "pois_table_tmp");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjUpdatePoisTable",
                        SequenceFileInputFormat.class,
                        AdjUpdatePoisTableReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poisTableWithInputIdPath, poiPairbtsIndexWithInputIdPath });
                FileOutputFormat.setOutputPath(job, poisTableTmpPath);
                job.waitForCompletion(true);
            }
            
            fs.delete(poisTablePath, true);
            fs.rename(poisTableTmpPath, poisTablePath);

            Path poiPairbtsAdjWithInputIdPath = new Path(tmpDirPath,
                    "poi_pairbts_adj_with_input_id");
            {
                ReduceJob job = ReduceJob.create(conf, "SetMobDataInputId",
                        SequenceFileInputFormat.class,
                        SetMobDataInputIdReducer.class,
                        SequenceFileOutputFormat.class);
                job.getConfiguration().setInt("input_id", 0);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job,
                        poiPairbtsAdjWithInputIdPath);
                job.waitForCompletion(true);
            }
            
            Path poiPairbtsCh1Path = new Path(tmpDirPath, "poi_pairbts_ch1");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSwapPoiIdSt1",
                        SequenceFileInputFormat.class,
                        AdjSwapPoiIdSt1Reducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poiPairbtsAdjWithInputIdPath,
                    poiPairbtsIndexWithInputIdPath });
                FileOutputFormat.setOutputPath(job, poiPairbtsCh1Path);
                job.waitForCompletion(true);
            }

            fs.delete(poiPairbtsAdjPath, true);
            fs.delete(poiPairbtsAdjWithInputIdPath, true);
            
            Path poiPairbtsCh1WithInputIdPath = new Path(tmpDirPath,
                    "poi_pairbts_ch1_with_input_id");
            {
                ReduceJob job = ReduceJob.create(conf, "SetMobDataInputId",
                        SequenceFileInputFormat.class,
                        SetMobDataInputIdReducer.class,
                        SequenceFileOutputFormat.class);
                job.getConfiguration().setInt("input_id", 0);
                FileInputFormat.setInputPaths(job, poiPairbtsCh1Path);
                FileOutputFormat.setOutputPath(job,
                        poiPairbtsCh1WithInputIdPath);
                job.waitForCompletion(true);
            }
            
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSwapPoiIdSt2",
                        SequenceFileInputFormat.class,
                        AdjSwapPoiIdSt2Reducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, new Path[] {
                    poiPairbtsCh1WithInputIdPath,
                    poiPairbtsIndexWithInputIdPath });
                FileOutputFormat.setOutputPath(job, poiPairbtsAdjPath);
                job.waitForCompletion(true);
            }
            
            Path nindSpreadPath = new Path(tmpDirPath, "nind_spread");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjSpreadCount",
                        SequenceFileInputFormat.class,
                        AdjSpreadCountReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, poiPairbtsAdjPath);
                FileOutputFormat.setOutputPath(job, nindSpreadPath);
                job.waitForCompletion(true);
            }

            Path numIndexPath = new Path(tmpDirPath, "num_index");
            {
                ReduceJob job = ReduceJob.create(conf, "AdjCountIndexes",
                        SequenceFileInputFormat.class,
                        AdjCountIndexesReducer.class,
                        SequenceFileOutputFormat.class);
                FileInputFormat.setInputPaths(job, nindSpreadPath);
                FileOutputFormat.setOutputPath(job, numIndexPath);
                job.waitForCompletion(true);
                numIndicesLeft = job.getCounters().findCounter(
                        Counters.NUM_INDEXES).getValue();
            }
        }
        
        Path poiPoimodPath = new Path(tmpDirPath, "poi_poimod");
        {
            MapJob job = MapJob.create(conf, "AdjSpreadTableByPoiId",
                    SequenceFileInputFormat.class,
                    AdjSpreadTableByPoiIdMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisTablePath);
            FileOutputFormat.setOutputPath(job, poiPoimodPath);
            job.waitForCompletion(true);
        }

        Path poiIdPoiPath = new Path(tmpDirPath, "poiId_poi");
        {
            MapJob job = MapJob.create(conf, "AdjSpreadPoisByPoiId",
                    SequenceFileInputFormat.class,
                    AdjSpreadPoisByPoiIdMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, poisIdPath);
            FileOutputFormat.setOutputPath(job, poiIdPoiPath);
            job.waitForCompletion(true);
        }
        
        Path pointsOfInterestModPath = new Path(tmpDirPath,
                                                "points_of_interest_mod");
        {
            ReduceJob job = ReduceJob.create(conf, "AdjJoinNewPoiId",
                    SequenceFileInputFormat.class,
                    AdjJoinNewPoiIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                poiPoimodPath, poiIdPoiPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestModPath);
            job.waitForCompletion(true);
        }
        
        {
            ReduceJob job = ReduceJob.create(conf, "AdjChangePoisId",
                    SequenceFileInputFormat.class,
                    AdjChangePoisIdReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] {
                pointsOfInterestPath, pointsOfInterestModPath });
            FileOutputFormat.setOutputPath(job, pointsOfInterestIdPath);
            job.waitForCompletion(true);
        }

        if (isDebug) {
            Path pointsOfInterestIdTextPath = new Path(tmpDirPath,
                    "points_of_interest_id_text");
            {
                ReduceJob job = ReduceJob.create(conf,
                        "ExportPoiToTextByTwoInt",
                        SequenceFileInputFormat.class,
                        ExportPoiToTextByTwoIntReducer.class,
                        1,
                        TextOutputFormat.class);
                FileInputFormat.setInputPaths(job, pointsOfInterestIdPath);
                FileOutputFormat.setOutputPath(job, pointsOfInterestIdTextPath);
                job.waitForCompletion(true);
            }
        }
    }
}
