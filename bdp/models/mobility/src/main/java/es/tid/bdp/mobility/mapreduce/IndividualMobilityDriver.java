package es.tid.bdp.mobility.mapreduce;

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

import es.tid.bdp.mobility.data.MobProtocol.GLEvent;

public class IndividualMobilityDriver extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(
            IndividualMobilityDriver.class);

    @Override
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

        if (!job.waitForCompletion(true)) {
            return 1;
        }
        
        return 0;
    }

    public static void main(final String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new IndividualMobilityDriver(),
                                     args);
            if (res != 0) {
                throw new Exception("Uknown failure");
            }
        } catch (Exception ex) {
            LOG.debug("ERROR ", ex);
            throw ex;
        }
    }
}
