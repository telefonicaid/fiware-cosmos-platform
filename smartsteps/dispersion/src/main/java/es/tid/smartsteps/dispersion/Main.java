package es.tid.smartsteps.dispersion;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.base.mapreduce.CosmosJob;

/**
 *
 * @author dmicol
 */
public class Main extends Configured implements Tool {
    private static final Logger LOGGER = Logger.getLogger(Main.class);

    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 4) {
            throw new IllegalArgumentException(
                    "Usage: trafficCountsPath cellToMicrogridPath "
                    + "microgridToPolygonPath outputDir");
        }

        final Configuration config = Config.load(Config.class.getResource(
                "/config.properties").openStream(), this.getConf());
        
        final Path trafficCountsPath = new Path(args[0]);
        final Path cellToMicrogridPath = new Path(args[1]);
        final Path microgridToPolygonPath = new Path(args[2]);
        final Path outputDir = new Path(args[3]);

        Path trafficCountsFilteredPath = new Path(outputDir,
                                                  "traffic_counts_filtered");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "DateFilter",
                    TextInputFormat.class,
                    DateFilterMapper.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsPath);
            FileOutputFormat.setOutputPath(job, trafficCountsFilteredPath);
            job.waitForCompletion(true);
        }
        
        Path countsByMicrogridPath = new Path(outputDir, "counts_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogrid",
                    TextInputFormat.class,
                    EntryScalerMapper.class,
                    EntryScalerReducer.class,
                    TextOutputFormat.class);
            job.getConfiguration().setEnum(LookupType.class.getName(),
                                           LookupType.CELL_TO_MICROGRID);
            FileInputFormat.setInputPaths(job, trafficCountsFilteredPath,
                                          cellToMicrogridPath);
            FileOutputFormat.setOutputPath(job, countsByMicrogridPath);
            job.waitForCompletion(true);
        }

        Path countsByPolygonPath = new Path(outputDir, "counts_by_polygon");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "MicrogridToPolygon",
                    TextInputFormat.class,
                    EntryScalerMapper.class,
                    EntryScalerReducer.class,
                    TextOutputFormat.class);
            job.getConfiguration().setEnum(LookupType.class.getName(),
                                           LookupType.MICROGRID_TO_POLYGON);
            FileInputFormat.setInputPaths(job, countsByMicrogridPath,
                                          microgridToPolygonPath);
            FileOutputFormat.setOutputPath(job, countsByPolygonPath);
            job.waitForCompletion(true);
        }
        
        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new Main(), args);
            if (res != 0) {
                throw new Exception("Uknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}
