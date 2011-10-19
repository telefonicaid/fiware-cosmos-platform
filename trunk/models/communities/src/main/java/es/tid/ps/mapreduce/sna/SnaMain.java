package es.tid.ps.mapreduce.sna;

import java.util.Date;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

/**
 * This class generate the social graph and the SNA from the cdrs. The process
 * are doing in two phases of map/reduce, and it is this class that configure
 * it.
 * 
 * TODO: it is necessary to change the configuration of hadoop cluster to a
 * configuration file, and it receives the the paths of the data from parameters
 * because now those data are into the code. Another problem are the output
 * files that the process generate, because it is necessary that these paths are
 * not in hadoop.
 */
public class SnaMain extends Configured implements Tool {

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new SnaMain(), args);
        System.exit(res);
    }

    public int run(String[] args) throws Exception {
        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1]);
        Path tmpPath = new Path("/user/rgc/tmp."
                + Long.toString(new Date().getTime()));

        Configuration conf = getConf();
        conf.set("mapred.reduce.tasks", "1");

        Job SocialGraphJob = new Job(conf, "Social graph ...");
        SocialGraphJob.setJarByClass(SnaMain.class);
        SocialGraphJob.setMapperClass(SocialGraphMapper.class);
        SocialGraphJob.setReducerClass(SocialGraphReducer.class);
        SocialGraphJob.setInputFormatClass(TextInputFormat.class);
        SocialGraphJob.setOutputKeyClass(Text.class);
        SocialGraphJob.setMapOutputValueClass(Text.class);
        SocialGraphJob.setOutputValueClass(ArrayListWritable.class);
        SocialGraphJob.setOutputFormatClass(SequenceFileOutputFormat.class);

        FileInputFormat.addInputPath(SocialGraphJob, inputPath);
        FileOutputFormat.setOutputPath(SocialGraphJob, tmpPath);

        // TODO(rgc): check if this funtionality is correct
        FileSystem hdfs = FileSystem.get(conf);
        hdfs.deleteOnExit(tmpPath);

        if (!SocialGraphJob.waitForCompletion(true)) {
            return 1;
        }

        Job snaJob = new Job(conf, "Sna ...");
        snaJob.setJarByClass(SnaMain.class);
        snaJob.setMapperClass(CliquesCalculatorMapper.class);
        snaJob.setReducerClass(CliquesCalculatorReducer.class);
        snaJob.setInputFormatClass(SequenceFileInputFormat.class);
        snaJob.setOutputKeyClass(Text.class);
        snaJob.setOutputValueClass(NodeCombination.class);

        FileInputFormat.addInputPath(snaJob, tmpPath);
        FileOutputFormat.setOutputPath(snaJob, outputPath);

        return snaJob.waitForCompletion(true) ? 0 : 1;
    }
}