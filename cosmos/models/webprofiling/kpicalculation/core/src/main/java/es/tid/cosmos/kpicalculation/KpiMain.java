package es.tid.cosmos.kpicalculation;

import java.io.IOException;
import java.net.URL;
import java.util.Date;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
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
import es.tid.cosmos.kpicalculation.config.KpiConfig;
import es.tid.cosmos.kpicalculation.config.KpiFeature;
import es.tid.cosmos.kpicalculation.export.mongodb.MongoDBExporterReducer;

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
    private static final Logger LOG = Logger.getLogger(KpiMain.class);
    private static final URL KPI_DEFINITIONS = KpiMain.class.getResource(
            "/kpi.properties");
    private static final int NUM_ARGS = 3;

    @Override
    public int run(String[] args) throws Exception {
        if (args.length != NUM_ARGS) {
            throw new IllegalArgumentException("Wrong Arguments. Example: "
                    + "hadoop jar kpicalculation.jar inputPath outputPath "
                    + "mongoURL");
        }

        FileSystem fs = FileSystem.get(this.getConf());

        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1] + "/aggregates");
        if (!fs.mkdirs(outputPath)) {
            throw new IOException("Could not create " + outputPath);
        }
        String timeFolder = "data." + Long.toString(new Date().getTime());
        Path tmpPath = new Path(args[1] + timeFolder + "/cleaned/");
        LOG.info("Using " + tmpPath + " as temp directory");
        String mongoUrl = args[2];

        Configuration conf = getConf();
        conf.set("kpicalculation.temp.path", tmpPath.toString());
        conf.addResource("kpi-filtering.xml");

        // Process that filters and formats input logs
        CosmosJob cleanerJob = CosmosJob.createMapJob(
                conf, "Web Profiling ...",
                LzoTextInputFormat.class,
                KpiCleanerMapper.class,
                SequenceFileOutputFormat.class);
        FileInputFormat.setInputPaths(cleanerJob, inputPath);
        FileOutputFormat.setOutputPath(cleanerJob, tmpPath);
        cleanerJob.setDeleteOutputOnExit(true);

        KpiConfig config = new KpiConfig();
        config.read(KPI_DEFINITIONS);

        WorkflowList wfList = new WorkflowList();
        for (KpiFeature features : config.getKpiFeatures()) {
            Path kpiOutputPath = outputPath.suffix("/" + timeFolder + "/"
                    + features.getName());

            CosmosJob aggregationJob = createAggregationJob(conf, features,
                    tmpPath, kpiOutputPath);
            aggregationJob.addDependentWorkflow(cleanerJob);

            CosmosJob exporterJob = CosmosJob.createReduceJob(conf, "MongoDBExporterJob",
                    TextInputFormat.class,
                    MongoDBExporterReducer.class,
                    MongoOutputFormat.class);
            Configuration exporterConf = exporterJob.getConfiguration();
            TextInputFormat.setInputPaths(exporterJob, kpiOutputPath);
            MongoConfigUtil.setOutputURI(exporterConf, mongoUrl);
            exporterConf.set("name", features.getName());
            exporterConf.setStrings("fields", features.getFields());
            exporterJob.addDependentWorkflow(aggregationJob);
            wfList.add(exporterJob);
        }
        wfList.waitForCompletion(true);

        return 0;
    }

    private CosmosJob createAggregationJob(Configuration conf,
                                           KpiFeature features,
                                           Path inputPath, Path outputPath)
            throws Exception {
        CosmosJob aggregationJob;
        if (features.getGroup() != null) {
            aggregationJob = CosmosJob.createMapReduceJob(conf,
                    "Aggregation Job ..." + features.getName(),
                    SequenceFileInputFormat.class,
                    KpiGenericMapper.class,
                    KpiCounterByReducer.class,
                    TextOutputFormat.class);
            aggregationJob.getConfiguration().setStrings(
                    "kpi.aggregation.group", features.getGroup());
            aggregationJob.setCombinerClass(KpiCounterByCombiner.class);
            aggregationJob.setSortComparatorClass(
                    PageViewKpiCounterGroupedComparator.class);
            aggregationJob.setGroupingComparatorClass(
                    PageViewKpiCounterGroupedComparator.class);
        } else {
            aggregationJob = CosmosJob.createMapReduceJob(conf,
                    "Aggregation Job ..." + features.getName(),
                    SequenceFileInputFormat.class,
                    KpiGenericMapper.class,
                    KpiCounterReducer.class,
                    TextOutputFormat.class);
            aggregationJob.setCombinerClass(KpiCounterCombiner.class);
            aggregationJob.setSortComparatorClass(
                    PageViewKpiCounterComparator.class);
            aggregationJob.setGroupingComparatorClass(
                    PageViewKpiCounterComparator.class);
        }
        aggregationJob.setPartitionerClass(KpiPartitioner.class);
        aggregationJob.getConfiguration().setStrings(
                "kpi.aggregation.fields", features.getFields());

        // Input and Output configuration
        FileInputFormat.setInputPaths(aggregationJob, inputPath);
        FileOutputFormat.setOutputPath(aggregationJob, outputPath);

        return aggregationJob;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(), new KpiMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOG.fatal(ex);
            throw ex;
        }
    }
}
