package es.tid.cosmos.mobility.preparing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;

/**
 *
 * @author dmicol
 */
public final class PreparingRunner {
    private PreparingRunner() {
    }

    public static void run(Path tmpPath, Path cdrsMobPath, Path cdrsInfoPath,
                           Path cdrsNoinfoPath, Path cellsPath,
                           Path clientsBtsPath, Path btsCommsPath,
                           Path cdrsNoBtsPath, Path viTelmonthBtsPath,
                           Configuration conf) throws Exception {
        {
            MapJob job = MapJob.create(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsInfoPath);
            job.waitForCompletion(true);
        }
        
        {
            MapJob job = MapJob.create(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsNoinfoPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToNodeBts",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToNodeBtsReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, clientsBtsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToBtsDayRange",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToBtsDayRangeReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, btsCommsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToCdr",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToCdrReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, cdrsNoBtsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf,
                    "JoinBtsNodeToTelMonthAndCell",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToTelMonthAndCellReducer.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().set("cells", cellsPath.toString());
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, viTelmonthBtsPath);
            job.waitForCompletion(true);
        }
    }
}
