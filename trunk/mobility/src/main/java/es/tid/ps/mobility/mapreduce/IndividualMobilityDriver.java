package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.ps.mobility.data.MobProtocol.GLEvent;

public class IndividualMobilityDriver extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(
            IndividualMobilityDriver.class);

    public int run(final String[] args) throws IOException,
                                               ClassNotFoundException,
                                               InterruptedException {
        final Configuration conf = getConf();
        final Job job = new Job(conf, "Mob2Productivization execution");

        job.setJarByClass(IndividualMobilityDriver.class);
        job.setMapperClass(IndividualMobilityMapper.class);
        job.setReducerClass(IndividualMobilityReducer.class);

        job.setOutputKeyClass(LongWritable.class);
        job.setOutputValueClass(GLEvent.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        return job.waitForCompletion(true) ? 0 : 1;
    }

    public static void main(final String[] args) {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new IndividualMobilityDriver(),
                                     args);
            System.exit(res);
        } catch (Exception e) {
            LOG.debug("ERROR ", e);
            System.exit(1);
        }
    }
}
