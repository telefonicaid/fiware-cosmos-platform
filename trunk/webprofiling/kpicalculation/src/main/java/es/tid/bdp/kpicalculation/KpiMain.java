package es.tid.bdp.kpicalculation;

import java.io.IOException;
import java.util.ArrayList;

import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

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

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.twitter.elephantbird.mapreduce.input.LzoProtobufB64LineInputFormat;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.mapreduce.output.LzoProtobufB64LineOutputFormat;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.base.mapreduce.SingleKey;
import es.tid.bdp.kpicalculation.data.JobDetails;
import es.tid.bdp.kpicalculation.data.KpiCalculationProtocol.WebProfilingLog;

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
    private static final Logger LOGGER = Logger.getLogger("KpiMain");

    public static void main(String[] args) {
        if (args.length != 2) {
            LOGGER.setLevel(Level.ALL);
            LOGGER.severe("Wrong Arguments. Example: hadoop jar kpicalculation-LocalBuild.jar inputPath outputPath");
            System.exit(1);
        }

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
        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1] + "/aggregates");
        String timeFolder = "data." + Long.toString(new Date().getTime());
        Path tmpPath = new Path(args[1] + "/cleaned/" + timeFolder);

        Configuration conf = getConf();
        conf.set("kpicalculation.temp.path", tmpPath.toString());
        conf.addResource("kpi-filtering.xml");

        // Process that filters and formats input logs
        Job job = cleanWebNavigationLogs(conf, inputPath, tmpPath);
        if (!job.waitForCompletion(true)) {
            return 1;
        }

        // Definition of kpis to calculate
        List<JobDetails> list = getKpiList();

        Iterator<JobDetails> it = list.iterator();

        while (it.hasNext()) {
            JobDetails jDet = it.next();
            Path kpiOutputPath = outputPath.suffix("/" + jDet.getName() + "/"
                    + timeFolder);
            Job aggregationJob = new Job(conf, "Aggregation Job ..."
                    + jDet.getName());

            aggregationJob.getConfiguration().setStrings(
                    "kpi.aggregation.fields", jDet.getFields());

            if (jDet.getGroup() != null) {
                aggregationJob.getConfiguration().setStrings(
                        "kpi.aggregation.group", jDet.getGroup());
                aggregationJob.setMapOutputKeyClass(BinaryKey.class);
                aggregationJob.setCombinerClass(KpiCounterByCombiner.class);
                aggregationJob.setReducerClass(KpiCounterByReducer.class);
                aggregationJob
                        .setSortComparatorClass(PageViewKpiCounterGroupedComparator.class);
                aggregationJob
                        .setGroupingComparatorClass(PageViewKpiCounterGroupedComparator.class);
            } else {
                aggregationJob.setMapOutputKeyClass(SingleKey.class);
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
            aggregationJob.setInputFormatClass(LzoProtobufB64LineInputFormat
                    .getInputFormatClass(WebProfilingLog.class,
                            aggregationJob.getConfiguration()));
            aggregationJob.setMapOutputValueClass(IntWritable.class);
            aggregationJob.setOutputKeyClass(Text.class);
            aggregationJob.setOutputValueClass(IntWritable.class);
            aggregationJob.setOutputFormatClass(TextOutputFormat.class);

            // Input and Output configuration
            FileInputFormat.addInputPath(aggregationJob, tmpPath);
            FileOutputFormat.setOutputPath(aggregationJob, kpiOutputPath);

            if (!aggregationJob.waitForCompletion(true)) {
                return 1;
            }
        }

        return 0;
    }

    private List<JobDetails> getKpiList() {
        List<JobDetails> list = new ArrayList<JobDetails>();

        list.add(new JobDetails("PAGE_VIEWS_PROT", "protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_VIS_DEV",
                "visitorId,protocol,device,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_VIS",
                "visitorId,protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DEV", "device,protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_MET", "method,protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_URL_VIS",
                "visitorId,urlDomain,urlPath,protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DOM_VIS",
                "visitorId,urlDomain,protocol,date"));
        list.add(new JobDetails("PAGE_VIEWS_PROT_DOM",
                "urlDomain,protocol,date"));
        list.add(new JobDetails("VISITORS_PROT_URL",
                "urlDomain,urlPath,protocol,date", "visitorId"));
        list.add(new JobDetails("VISITORS_PROT", "protocol,date", "visitorId"));

        return list;
    }

    private Job cleanWebNavigationLogs(Configuration conf, Path input,
            Path output) throws IOException, InterruptedException,
            ClassNotFoundException {
        // Normalization and filtering

        // Job configuration
        Job wpCleanerJob = new Job(conf, "Web Profiling ...");
        wpCleanerJob.setNumReduceTasks(0);
        wpCleanerJob.setJarByClass(KpiMain.class);
        wpCleanerJob.setMapperClass(KpiCleanerMapper.class);
        wpCleanerJob.setInputFormatClass(LzoTextInputFormat.class);
        wpCleanerJob.setOutputKeyClass(NullWritable.class);
        wpCleanerJob.setOutputValueClass(ProtobufWritable.class);
        wpCleanerJob.setMapOutputKeyClass(NullWritable.class);
        wpCleanerJob.setMapOutputValueClass(ProtobufWritable.class);
        wpCleanerJob.setOutputFormatClass(LzoProtobufB64LineOutputFormat
                .getOutputFormatClass(WebProfilingLog.class,
                        wpCleanerJob.getConfiguration()));

        // Input and Output configuration
        FileInputFormat.addInputPath(wpCleanerJob, input);
        FileOutputFormat.setOutputPath(wpCleanerJob, output);

        return wpCleanerJob;

    }
}
