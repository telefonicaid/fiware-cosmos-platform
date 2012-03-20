package es.tid.bdp.kpicalculation;

import java.io.IOException;
import java.net.URL;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.twitter.elephantbird.mapreduce.input.LzoProtobufB64LineInputFormat;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.mapreduce.output.LzoProtobufB64LineOutputFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.base.mapreduce.SingleKey;
import es.tid.bdp.kpicalculation.config.KpiFeature;
import es.tid.bdp.kpicalculation.config.KpiConfig;
import es.tid.bdp.kpicalculation.export.mongodb.MongoDBExporterJob;
import es.tid.bdp.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;

/**
 * This class performs the webprofiling processing of the data received from
 * dpis and calculates different kpis of about that data. The process is a chain
 * of map & reduces operations that will generate the final results.
 *
 * TODO: At the moment due to problems using the jdbc-hive connector directly,
 * the hive server must be used. This should be reviewed in the future. All the
 * queries are added sequentially and will need to be extracted to a properties
 * file in order to modify the process without the need of recompiling the
 * project
 */
public class KpiMain extends Configured implements Tool {
    private static final URL KPI_DEFINITIONS = KpiMain.class.getResource(
            "/kpi.properties");
    private static final Logger LOGGER = Logger.getLogger("KpiMain");
    private static final int NUM_ARGS = 3;

    public static void main(String[] args) {
        try {
            int res = ToolRunner.run(new Configuration(), new KpiMain(), args);
            System.exit(res);
        } catch (Exception e) {
            LOGGER.setLevel(Level.ALL);
            LOGGER.log(Level.SEVERE, "Execution failed: {0}", e.getMessage());
            System.exit(1);
        }
    }

    @Override
    public int run(String[] args) throws IOException, ClassNotFoundException,
                                         InterruptedException {
        if (args.length != NUM_ARGS) {
            LOGGER.setLevel(Level.ALL);
            LOGGER.severe("Wrong Arguments. Example: hadoop jar "
                    + "kpicalculation.jar inputPath outputPath mongoURL");
            return 1;
        }

        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1] + "/aggregates");
        String timeFolder = "data." + Long.toString(new Date().getTime());
        Path tmpPath = new Path(args[1] + timeFolder + "/cleaned/");
        String mongoUrl = args[2];

        Configuration conf = getConf();
        conf.set("kpicalculation.temp.path", tmpPath.toString());
        conf.addResource("kpi-filtering.xml");

        // Process that filters and formats input logs
        Job job = cleanWebNavigationLogs(conf, inputPath, tmpPath);
        if (!job.waitForCompletion(true)) {
            return 1;
        }

        KpiConfig config = new KpiConfig();
        config.read(KPI_DEFINITIONS);

        for (KpiFeature features : config.getKpiFeatures()) {
            Path kpiOutputPath = outputPath.suffix("/" + timeFolder + "/"
                    + features.getName());
            Job aggregationJob = createAggregationJob(conf, features,
                                                      tmpPath, kpiOutputPath);
            if (!aggregationJob.waitForCompletion(true)) {
                return 1;
            }

            String mongoCollectionUrl = mongoUrl;
            if (!mongoCollectionUrl.endsWith("/")) {
                mongoCollectionUrl += "/";
            }
            mongoCollectionUrl += features.getName();
            MongoDBExporterJob exporterJob = new MongoDBExporterJob(conf);
            exporterJob.configure(kpiOutputPath, mongoCollectionUrl);
            if (!exporterJob.waitForCompletion(true)) {
                return 1;
            }
        }

        return 0;
    }

    private Job cleanWebNavigationLogs(Configuration conf, Path input,
                                       Path output)
            throws IOException, InterruptedException, ClassNotFoundException {
        Job wpCleanerJob = new Job(conf, "Web Profiling ...");
        wpCleanerJob.setNumReduceTasks(0);
        wpCleanerJob.setJarByClass(KpiMain.class);
        wpCleanerJob.setMapperClass(KpiCleanerMapper.class);
        wpCleanerJob.setInputFormatClass(LzoTextInputFormat.class);
        wpCleanerJob.setOutputKeyClass(NullWritable.class);
        wpCleanerJob.setOutputValueClass(ProtobufWritable.class);
        wpCleanerJob.setMapOutputKeyClass(NullWritable.class);
        wpCleanerJob.setMapOutputValueClass(ProtobufWritable.class);
        wpCleanerJob.setOutputFormatClass(
                LzoProtobufB64LineOutputFormat.getOutputFormatClass(
                        WebProfilingLog.class,
                        wpCleanerJob.getConfiguration()));

        FileInputFormat.addInputPath(wpCleanerJob, input);
        FileOutputFormat.setOutputPath(wpCleanerJob, output);

        return wpCleanerJob;
    }

    private Job createAggregationJob(Configuration conf, KpiFeature features,
                                     Path inputPath, Path outputPath)
                                     throws IOException {
        Job aggregationJob = new Job(conf, "Aggregation Job ..."
                + features.getName());

        aggregationJob.getConfiguration().setStrings(
                "kpi.aggregation.fields", features.getFields());

        if (features.getGroup() != null) {
            aggregationJob.getConfiguration().setStrings(
                    "kpi.aggregation.group", features.getGroup());
            aggregationJob.setMapOutputKeyClass(BinaryKey.class);
            aggregationJob.setCombinerClass(KpiCounterByCombiner.class);
            aggregationJob.setReducerClass(KpiCounterByReducer.class);
            aggregationJob.setSortComparatorClass(
                    PageViewKpiCounterGroupedComparator.class);
            aggregationJob.setGroupingComparatorClass(
                    PageViewKpiCounterGroupedComparator.class);
        } else {
            aggregationJob.setMapOutputKeyClass(SingleKey.class);
            aggregationJob.setCombinerClass(KpiCounterCombiner.class);
            aggregationJob.setReducerClass(KpiCounterReducer.class);
            aggregationJob.setSortComparatorClass(
                    PageViewKpiCounterComparator.class);
            aggregationJob.setGroupingComparatorClass(
                    PageViewKpiCounterComparator.class);
        }

        aggregationJob.setJarByClass(KpiMain.class);
        aggregationJob.setMapperClass(KpiGenericMapper.class);
        aggregationJob.setPartitionerClass(KpiPartitioner.class);
        aggregationJob.setInputFormatClass(
                LzoProtobufB64LineInputFormat.getInputFormatClass(
                        WebProfilingLog.class,
                        aggregationJob.getConfiguration()));
        aggregationJob.setMapOutputValueClass(IntWritable.class);
        aggregationJob.setOutputKeyClass(Text.class);
        aggregationJob.setOutputValueClass(IntWritable.class);
        aggregationJob.setOutputFormatClass(TextOutputFormat.class);

        // Input and Output configuration
        FileInputFormat.addInputPath(aggregationJob, inputPath);
        FileOutputFormat.setOutputPath(aggregationJob, outputPath);
        
        return aggregationJob;
    }
}
