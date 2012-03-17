package es.tid.mapreduce.mobility2;

import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.mapreduce.mobility2.data.UserMobilityData;

/**
 * This class generates the mobility of the Telefonica User from de cdrs,
 * Data-Users and Data-Cells. are doing in one map/reduce, and it is this class
 * that configure it. This is the implementation of "Mobility 2.0", during the
 * process we join up all the CDRs by users, then we calculate the PDIs of the
 * user ...
 * 
 * TODO rgc finished the comment when he have the requirements.
 * */
public class Mobility2Main extends Configured implements Tool {

    public static void main(String[] args) throws Exception {
        int res = ToolRunner
                .run(new Configuration(), new Mobility2Main(), args);
        System.exit(res);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.util.Tool#run(java.lang.String[])
     */
    public int run(String[] args) throws Exception {
        Path cdrsPath = new Path(args[0]);
        Path outputPath = new Path(args[1]);

        Configuration conf = new Configuration();
        Job mobility2Job = new Job(conf, "Mobility2 ...");

        // TODO rgc: change this values to input values from a configuration
        // file

        DistributedCache.addCacheFile(new URI(
                "/user/rgc/cache/GASSET_CELDAS_RED_OCT2009.dat"), mobility2Job
                .getConfiguration());

        // TODO rgc: Distributed cache read directly from HDFS, in every
        // process, the best way will be distribute the data from every node
        // before start the process
        // DistributedCache.addCacheFile(new URI(""),
        // mobilityJob.getConfiguration());
        // DistributedCache.createSymlink(mobilityJob.getConfiguration());
        // DistributedCache.createSymlink(conf);

        // TODO rgc: check how upload diferent types of files to the cache

        mobility2Job.setJarByClass(Mobility2Main.class);
        mobility2Job.setReducerClass(Mobility2Reducer.class);
        mobility2Job.setMapOutputKeyClass(Text.class);
        mobility2Job.setMapOutputValueClass(UserMobilityData.class);
        mobility2Job.setOutputKeyClass(Text.class);
        mobility2Job.setOutputValueClass(Text.class);

        FileInputFormat.addInputPath(mobility2Job, cdrsPath);
        FileOutputFormat.setOutputPath(mobility2Job, outputPath);

        return mobility2Job.waitForCompletion(true) ? 0 : 1;
    }
}