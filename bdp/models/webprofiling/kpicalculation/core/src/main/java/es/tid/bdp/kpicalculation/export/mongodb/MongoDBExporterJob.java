package es.tid.bdp.kpicalculation.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.io.BSONWritable;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

import es.tid.bdp.kpicalculation.KpiMain;
import es.tid.bdp.kpicalculation.config.KpiFeature;

/**
 * Export user profiles to mongodb
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterJob extends Job {
    private static final String JOB_NAME = "MongoDBExporterJob";

    public MongoDBExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(KpiMain.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setReducerClass(MongoDBExporterReducer.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(BSONWritable.class);
    }

    /**
     * Configure inputs and outputs.
     *
     * @param inputPath HDFS input path
     * @param outputUrl Mongodb output url,
     *                  for instance mongodb://host/db.collection
     * @throws IOException
     */
    public void configure(Path inputPath, String outputUrl, KpiFeature feature)
            throws IOException {
        this.setInputFormatClass(TextInputFormat.class);
        TextInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(MongoOutputFormat.class);
        MongoConfigUtil.setOutputURI(this.conf, outputUrl);
        this.conf.setStrings("fields", feature.getFields());
    }
}
