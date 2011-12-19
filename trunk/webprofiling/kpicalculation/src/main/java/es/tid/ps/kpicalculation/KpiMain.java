package es.tid.ps.kpicalculation;

import java.util.ArrayList;

import java.util.Date;
import java.util.Iterator;
import java.util.List;

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

import com.hadoop.compression.lzo.LzopCodec;
import com.hadoop.mapreduce.LzoTextInputFormat;

import es.tid.ps.kpicalculation.data.JobDetails;
import es.tid.ps.kpicalculation.data.WebLogFactory;
import es.tid.ps.kpicalculation.data.WebLogType;

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
    private static String TEMP_PATH = "/user/javierb/temp";
    private static String OUTPUT_PATH = "/user/javierb/webprofiling/aggregates";

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new KpiMain(), args);
        System.exit(res);
    }

    public int run(String[] args) throws Exception {
        Path inputPath = new Path(args[0]);
        Path outputPath;
        Date date = new Date();
        Path tmpPath = new Path(TEMP_PATH + "/tmp."
                + Long.toString(new Date().getTime()));

        // Normalization and filtering
        Configuration conf = getConf();
        conf.set("kpicalculation.temp.path", tmpPath.toString());
        conf.addResource("kpi-filtering.xml");

        // Job configuration
        Job wpCleanerJob = new Job(conf, "Web Profiling ...");
        wpCleanerJob.setNumReduceTasks(0);
        wpCleanerJob.setJarByClass(KpiMain.class);
        wpCleanerJob.setMapperClass(KpiCleanerMapper.class);
        wpCleanerJob.setInputFormatClass(LzoTextInputFormat.class);
        wpCleanerJob.setOutputKeyClass(Text.class);
        wpCleanerJob.setMapOutputValueClass(Text.class);
        wpCleanerJob.setMapOutputKeyClass(NullWritable.class);
        wpCleanerJob.setOutputValueClass(Text.class);
        wpCleanerJob.setOutputFormatClass(TextOutputFormat.class);

        // Input and Output configuration
        FileInputFormat.addInputPath(wpCleanerJob, inputPath);
        FileOutputFormat.setCompressOutput(wpCleanerJob, true);
        FileOutputFormat
                .setOutputCompressorClass(wpCleanerJob, LzopCodec.class);
        FileOutputFormat.setOutputPath(wpCleanerJob, tmpPath);

        if (!wpCleanerJob.waitForCompletion(true)) {
            return 1;
        }

        // Definition of kpis to calculate
        List<JobDetails> list = new ArrayList<JobDetails>();

        list.add(new JobDetails("PAGE_VIEWS_PROT", "protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_VIS_DEV",
                "visitorId,protocol,device,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_VIS",
                "visitorId,protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DEV",
                "device,protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_MET",
                "method,protocol,dateView"));
        //list.add(new JobDetails("PAGE_VIEWS_PROT", "protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_URL_VIS",
                "visitorId,urlDomain,urlPath,protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DOM_VIS",
                "visitorId,urlDomain,protocol,dateView"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DOM",
                "urlDomain,protocol,dateView"));

        list.add(new JobDetails("VISITORS_PROT_URL",
                "urlDomain,urlPath,protocol,dateView", "visitorId"));
        list.add(new JobDetails("VISITORS_PROT", "protocol,dateView",
                "visitorId"));

        Iterator<JobDetails> it = list.iterator();

        while (it.hasNext()) {
            JobDetails jDet = it.next();
            outputPath = new Path(OUTPUT_PATH).suffix("/" + jDet.getName()
                    + "/" + date.getYear() + "/" + date.getMonth() + "/"
                    + date.getDay());
            Job aggregationJob = new Job(conf, "Aggregation Job ..."
                    + jDet.getName());

            aggregationJob.getConfiguration().setStrings(
                    "kpi.aggregation.fields", jDet.getFields());

            if (jDet.getGroup() != null) {
                aggregationJob.getConfiguration().setStrings(
                        "kpi.aggregation.group", jDet.getGroup());
                aggregationJob.getConfiguration().setStrings(
                        "kpi.aggregation.type",
                        WebLogType.WEB_LOG_COUNTER_GROUP.toString());
                WebLogFactory.setKeys(
                        aggregationJob.getConfiguration().getStringCollection(
                                "kpi.aggregation.fields"),
                        aggregationJob.getConfiguration().get(
                                "kpi.aggregation.group"),
                        WebLogType.WEB_LOG_COUNTER_GROUP);

                aggregationJob.setCombinerClass(KpiCounterByCombiner.class);
                aggregationJob.setReducerClass(KpiCounterByReducer.class);
                aggregationJob
                        .setSortComparatorClass(PageViewKpiCounterGroupedComparator.class);
                aggregationJob
                        .setGroupingComparatorClass(PageViewKpiCounterGroupedComparator.class);
            } else {
                aggregationJob.getConfiguration().setStrings(
                        "kpi.aggregation.type",
                        WebLogType.WEB_LOG_COUNTER.toString());
                WebLogFactory.setKeys(
                        aggregationJob.getConfiguration().getStringCollection(
                                "kpi.aggregation.fields"),
                        aggregationJob.getConfiguration().get(
                                "kpi.aggregation.group"),
                        WebLogType.WEB_LOG_COUNTER);
                aggregationJob.setCombinerClass(KpiCounterCombiner.class);
                aggregationJob.setReducerClass(KpiCounterReducer.class);
                aggregationJob
                        .setSortComparatorClass(PageViewKpiCounterComparator.class);
                aggregationJob
                        .setGroupingComparatorClass(PageViewKpiCounterComparator.class);
            }

            aggregationJob.setJarByClass(KpiMain.class);
            aggregationJob.setMapperClass(KpiGenericMapper.class);
            aggregationJob.setPartitionerClass(KpiPartitioner.class);

            aggregationJob.setInputFormatClass(LzoTextInputFormat.class);

            aggregationJob.setMapOutputKeyClass(WebLogFactory.getPageView()
                    .getClass());
            aggregationJob.setMapOutputValueClass(IntWritable.class);
            aggregationJob.setOutputKeyClass(Text.class);
            aggregationJob.setOutputValueClass(IntWritable.class);
            aggregationJob.setOutputFormatClass(TextOutputFormat.class);

            // Input and Output configuration
            FileInputFormat.addInputPath(aggregationJob, tmpPath);
            // FileOutputFormat.setCompressOutput(aggregationJob, true);
            // FileOutputFormat.setOutputCompressorClass(aggregationJob,
            // LzopCodec.class);
            FileOutputFormat.setOutputPath(aggregationJob, outputPath);

            if (!aggregationJob.waitForCompletion(true)) {
                return 1;
            }
        }

        /*
         * // Data load into hive ICdrLoader loader = new HiveCdrLoader(conf);
         * loader.load(TEMP_PATH);
         * 
         * // Calculation of aggregate data IAggregateCalculator agg = new
         * HiveAggregateCalculator(); agg.process();
         */
        return 0;
    }
}
