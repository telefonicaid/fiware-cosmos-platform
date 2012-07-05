package es.tid.smartsteps.dispersion;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
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
        
        Path trafficCountsParsedPath = new Path(outputDir,
                                                "traffic_counts_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "TrafficCountsParser",
                    TextInputFormat.class,
                    TrafficCountsParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsPath);
            FileOutputFormat.setOutputPath(job, trafficCountsParsedPath);
            job.waitForCompletion(true);
        }

        boolean shouldFilterByDate = !config.get(Config.DATE_TO_FILTER).isEmpty();
        Path trafficCountsParsedFilteredPath = new Path(outputDir,
                "traffic_counts_parsed_filtered");
        if (shouldFilterByDate) {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "DateFilter",
                    SequenceFileInputFormat.class,
                    DateFilterMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsParsedPath);
            FileOutputFormat.setOutputPath(job, trafficCountsParsedFilteredPath);
            job.waitForCompletion(true);
        }
        
        Path cellToMicrogridParsedPath = new Path(outputDir,
                                                  "cell_to_microgrid_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "CellToMicrogridLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellToMicrogridPath);
            FileOutputFormat.setOutputPath(job, cellToMicrogridParsedPath);
            job.waitForCompletion(true);
        }
        
        Path countsByMicrogridPath = new Path(outputDir, "counts_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogridEntryScaler",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TrafficCountsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, shouldFilterByDate ?
                    trafficCountsParsedFilteredPath : trafficCountsParsedPath,
                    cellToMicrogridParsedPath);
            FileOutputFormat.setOutputPath(job, countsByMicrogridPath);
            job.waitForCompletion(true);
        }

        Path microgridToPolygonParsedPath = new Path(outputDir,
                "microgrid_to_polygon_parsed");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "MicrogridToPolygonLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, microgridToPolygonPath);
            FileOutputFormat.setOutputPath(job, microgridToPolygonParsedPath);
            job.waitForCompletion(true);
        }
        
        Path countsByPolygonPath = new Path(outputDir, "counts_by_polygon");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "MicrogridToPolygonEntryScaler",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TrafficCountsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, countsByMicrogridPath,
                                          microgridToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, countsByPolygonPath);
            job.waitForCompletion(true);
        }

        Path aggregatedCountsByPolygonPath = new Path(outputDir,
                "aggregated_counts_by_polygon");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "AggregationByCellIdAndDate",
                    SequenceFileInputFormat.class,
                    CellIdAndDateMapper.class,
                    AggregationReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, countsByPolygonPath);
            FileOutputFormat.setOutputPath(job, aggregatedCountsByPolygonPath);
            job.waitForCompletion(true);
        }

        Path aggregatedCountsByPolygonTextPath = new Path(outputDir,
                "aggregated_counts_by_polygon_text");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "AggregationByCellIdAndDate",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, aggregatedCountsByPolygonPath);
            FileOutputFormat.setOutputPath(job, aggregatedCountsByPolygonTextPath);
            job.waitForCompletion(true);
        }
        
        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new Main(), args);
            if (res != 0) {
                throw new Exception("Uknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}
