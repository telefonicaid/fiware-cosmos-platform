package es.tid.ps.mapreduce.mobility;

import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;

/**
 * This class generates the mobility of the Telefonica User from de cdrs,
 * Data-Users and Data-Cells. are doing in one map/reduce, and it is this class
 * that configure it. This is the implementation of "Mobility 1.0", during the
 * process we join the cdrs with the Data Users and disscart all lines that the
 * users is not in the list, then clasificate the cdrs by time into "home_time"
 * and work_time, at the end agregate the data by users and cell_id.
 * */
public class MobilityMain extends Configured implements Tool {

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new MobilityMain(), args);
        System.exit(res);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.util.Tool#run(java.lang.String[])
     */
    public int run(String[] args) throws Exception {
        Path usersPath = new Path(args[0]);
        Path cdrsPath = new Path(args[1]);
        Path outputPath = new Path(args[2]);

        Configuration conf = new Configuration();
        Job mobilityJob = new Job(conf, "Mobility ...");

        // TODO rgc: change this values to input values from a configuration
        // file
        mobilityJob.getConfiguration().set("mapred.reduce.tasks", "1");
        mobilityJob.getConfiguration().set("models.mobility.home.cron",
                "* * 8-15 ? * 1-4");
        mobilityJob.getConfiguration().set("models.mobility.work.cron",
                "* * 16-23 ? * 1-7");
        DistributedCache.addCacheFile(new URI(
                "/user/rgc/cache/GASSET_CELDAS_RED_OCT2009.dat"), mobilityJob
                .getConfiguration());

        // TODO rgc: Distributed cache read directly from HDFS, in every
        // process, the best way will be distribute the data from every node
        // before start the process
        // DistributedCache.addCacheFile(new URI(""),
        // mobilityJob.getConfiguration());
        // DistributedCache.createSymlink(mobilityJob.getConfiguration());
        // DistributedCache.createSymlink(conf);

        mobilityJob.setJarByClass(MobilityMain.class);
        mobilityJob.setReducerClass(MobilityReducer.class);
        mobilityJob.setMapOutputKeyClass(CompositeKey.class);
        mobilityJob.setMapOutputValueClass(Text.class);
        mobilityJob.setOutputKeyClass(Text.class);
        mobilityJob.setOutputValueClass(LongWritable.class);

        MultipleInputs.addInputPath(mobilityJob, usersPath,
                TextInputFormat.class, JoinUserMapper.class);
        MultipleInputs.addInputPath(mobilityJob, cdrsPath,
                TextInputFormat.class, JoinCdrsMapper.class);

        FileOutputFormat.setOutputPath(mobilityJob, outputPath);
        mobilityJob.setSortComparatorClass(NaturalKeySortComparator.class);
        mobilityJob
                .setGroupingComparatorClass(NaturalKeySortComparator.class);

        return mobilityJob.waitForCompletion(true) ? 0 : 1;
    }
}