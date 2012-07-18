package es.tid.smartsteps.footfalls;

import java.io.IOException;

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
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.smartsteps.footfalls.aggregation.AggregationReducer;
import es.tid.smartsteps.footfalls.aggregation.CellIdAndDateMapper;
import es.tid.smartsteps.footfalls.catchments.*;
import es.tid.smartsteps.footfalls.centroids.CentroidJoinerMapper;
import es.tid.smartsteps.footfalls.centroids.CentroidJoinerReducer;
import es.tid.smartsteps.footfalls.centroids.CentroidParserMapper;
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.lookups.LookupParserMapper;
import es.tid.smartsteps.footfalls.lookups.LookupRekeyerMapper;
import es.tid.smartsteps.footfalls.lookups.LookupRekeyerMapper.RekeyBy;
import es.tid.smartsteps.footfalls.lookups.TransitiveLookupReducer;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsJsonExporterReducer;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsParserMapper;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsScalerMapper;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsScalerReducer;

/**
 *
 * @author dmicol
 */
public class Main extends Configured implements Tool {

    private static final Logger LOGGER = Logger.getLogger(Main.class);

    private enum Type {

        TRAFFIC_COUNTS,
        CATCHMENTS
    }

    @Override
    public int run(String[] args) throws ClassNotFoundException,
                                         InterruptedException, IOException {

        if (args.length != 7) {
            throw new IllegalArgumentException(
                    "Usage: [traffic_counts|catchments] trafficCountsPath "
                    + "cellToMicrogridPath microgridToPolygonPath "
                    + "soaCentroidsPath catchmentsPath outputDir");
        }

        final Configuration config = Config.load(Config.class.getResource(
                "/config.properties").openStream(), this.getConf());

        final Type type = Type.valueOf(args[0].toUpperCase());
        final Path trafficCountsPath = new Path(args[1]);
        final Path cellToMicrogridPath = new Path(args[2]);
        final Path microgridToPolygonPath = new Path(args[3]);
        final Path soaCentroidsPath = new Path(args[4]);
        final Path catchmentsPath = new Path(args[5]);
        final Path outputDir = new Path(args[6]);

        final Path trafficCountsParsedPath = new Path(outputDir,
                "traffic_counts_parsed");
        final Path cellToMicrogridParsedPath = new Path(outputDir,
                "cell_to_microgrid_parsed");
        final Path microgridToPolygonParsedPath = new Path(outputDir,
                "microgrid_to_polygon_parsed");
        final Path soaCentroidsParsedPath = new Path(outputDir,
                                                     "soa_centroids_parsed");
        final Path catchmentsParsedPath = new Path(outputDir,
                                                   "catchments_parsed");

        parse(trafficCountsPath, trafficCountsParsedPath, cellToMicrogridPath,
              cellToMicrogridParsedPath, microgridToPolygonPath,
              microgridToPolygonParsedPath, soaCentroidsPath,
              soaCentroidsParsedPath, catchmentsPath, catchmentsParsedPath,
              config);

        switch (type) {
            case TRAFFIC_COUNTS:
                propagateTrafficCounts(trafficCountsParsedPath,
                        cellToMicrogridParsedPath, microgridToPolygonParsedPath,
                        soaCentroidsParsedPath, outputDir, config);
                break;
            case CATCHMENTS:
                propagateCatchments(catchmentsParsedPath,
                        cellToMicrogridParsedPath, microgridToPolygonParsedPath,
                        outputDir, config);
                break;
            default:
                throw new IllegalArgumentException("Invalid execution mode");
        }

        return 0;
    }

    private static void parse(
            Path trafficCountsPath, Path trafficCountsParsedPath,
            Path cellToMicrogridPath, Path cellToMicrogridParsedPath,
            Path microgridToPolygonPath, Path microgridToPolygonParsedPath,
            Path soaCentroidsPath, Path soaCentroidsParsedPath,
            Path catchmentsPath, Path catchmentsParsedPath, Configuration config)
            throws ClassNotFoundException, IOException, InterruptedException {
        WorkflowList wfl = new WorkflowList();

        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "TrafficCountsParser",
                    TextInputFormat.class,
                    TrafficCountsParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsPath);
            FileOutputFormat.setOutputPath(job, trafficCountsParsedPath);
            wfl.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "CellToMicrogridLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellToMicrogridPath);
            FileOutputFormat.setOutputPath(job, cellToMicrogridParsedPath);
            wfl.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "MicrogridToPolygonLookupParser",
                    TextInputFormat.class,
                    LookupParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, microgridToPolygonPath);
            FileOutputFormat.setOutputPath(job, microgridToPolygonParsedPath);
            wfl.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "SOACentroidParser",
                    TextInputFormat.class,
                    CentroidParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, soaCentroidsPath);
            FileOutputFormat.setOutputPath(job, soaCentroidsParsedPath);
            wfl.add(job);
        }

        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "CatchmentsParser",
                    TextInputFormat.class,
                    CatchmentsParserMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsPath);
            FileOutputFormat.setOutputPath(job, catchmentsParsedPath);
            wfl.add(job);
        }

        wfl.waitForCompletion(true);
    }

    private static void propagateTrafficCounts(
            Path trafficCountsParsedPath, Path cellToMicrogridParsedPath,
            Path microgridToPolygonParsedPath, Path soaCentroidsParsedPath,
            Path outputDir, Configuration config)
            throws ClassNotFoundException, IOException, InterruptedException {
        Path countsByMicrogridPath = new Path(outputDir, "counts_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogridEntryScaler",
                    SequenceFileInputFormat.class,
                    TrafficCountsScalerMapper.class,
                    TrafficCountsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, trafficCountsParsedPath,
                                          cellToMicrogridParsedPath);
            FileOutputFormat.setOutputPath(job, countsByMicrogridPath);
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

        Path aggregatedCountsByPolygonJoinedPath = new Path(outputDir,
                "soa_centroids_parsed_joined");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "SOACentroidJoiner",
                    SequenceFileInputFormat.class,
                    CentroidJoinerMapper.class,
                    CentroidJoinerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, aggregatedCountsByPolygonPath,
                                          soaCentroidsParsedPath);
            FileOutputFormat.setOutputPath(job,
                                           aggregatedCountsByPolygonJoinedPath);
            job.waitForCompletion(true);
        }

        Path aggregatedCountsByPolygonJoinedTextPath = new Path(outputDir,
                "aggregated_counts_by_polygon_joined_text");
        {
            CosmosJob job = CosmosJob.createReduceJob(config,
                    "TrafficCountsJsonExporter",
                    SequenceFileInputFormat.class,
                    TrafficCountsJsonExporterReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job,
                                          aggregatedCountsByPolygonJoinedPath);
            FileOutputFormat.setOutputPath(job,
                    aggregatedCountsByPolygonJoinedTextPath);
            job.waitForCompletion(true);
        }
    }

    private static void propagateCatchments(
            Path catchmentsParsedPath, Path cellToMicrogridParsedPath,
            Path microgridToPolygonParsedPath,Path outputDir, Configuration config)
            throws ClassNotFoundException, IOException, InterruptedException {
        Path cellToMicrogridParsedRekeyedByValuePath = new Path(outputDir,
                "cell_to_microgrid_parsed_rekeyed_by_value");
        {
            CosmosJob job = CosmosJob.createMapJob(config,
                    "LookupRekeyer",
                    SequenceFileInputFormat.class,
                    LookupRekeyerMapper.class,
                    SequenceFileOutputFormat.class);
            job.getConfiguration().setEnum(RekeyBy.class.getName(), RekeyBy.VALUE);
            FileInputFormat.setInputPaths(job, cellToMicrogridParsedPath);
            FileOutputFormat.setOutputPath(job,
                    cellToMicrogridParsedRekeyedByValuePath);
            job.waitForCompletion(true);
        }

        Path cellToPolygonParsedPath = new Path(outputDir,
                                                "cell_to_polygon_parsed");
        {
            CosmosJob job = CosmosJob.createReduceJob(config,
                    "TransitiveLookup",
                    SequenceFileInputFormat.class,
                    TransitiveLookupReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job,
                    cellToMicrogridParsedRekeyedByValuePath,
                    microgridToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, cellToPolygonParsedPath);
            job.waitForCompletion(true);
        }

        Path catchmentsByMicrogridPath = new Path(outputDir,
                                                  "catchments_by_microgrid");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "CellToMicrogridCatchmentsScaler",
                    SequenceFileInputFormat.class,
                    CatchmentsScalerMapper.class,
                    CatchmentsScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsParsedPath,
                                          cellToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, catchmentsByMicrogridPath);
            job.waitForCompletion(true);
        }

        Path scaledTopCellsPath = new Path(outputDir, "scaled_topcells");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "ScaleTopCells",
                    SequenceFileInputFormat.class,
                    TopCellScalerMapper.class,
                    TopCellScalerReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, catchmentsByMicrogridPath,
                                          cellToPolygonParsedPath);
            FileOutputFormat.setOutputPath(job, scaledTopCellsPath);
            job.waitForCompletion(true);
        }

        Path propagatedCatchments = new Path(outputDir, "propagated_catchments");
        {
            CosmosJob job = CosmosJob.createMapReduceJob(config,
                    "AggregateTopCells",
                    SequenceFileInputFormat.class,
                    TopCellAggregatorMapper.class,
                    TopCellAggregatorReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, scaledTopCellsPath);
            FileOutputFormat.setOutputPath(job, propagatedCatchments);
            job.waitForCompletion(true);
        }

        Path soaCatchments = new Path(outputDir, "soa_catchments");
        {
            CosmosJob job = CosmosJob.createReduceJob(config,
                    "CatchmetnsJsonExporter",
                    SequenceFileInputFormat.class,
                    CatchmentsJsonExporterReducer.class,
                    1,
                    TextOutputFormat.class);
            FileInputFormat.setInputPaths(job, propagatedCatchments);
            FileOutputFormat.setOutputPath(job, soaCatchments);
            job.waitForCompletion(true);
        }
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new Main(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}
