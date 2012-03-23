package es.tid.bdp.recomms.importer;

import java.io.IOException;

import com.mongodb.BasicDBObject;
import com.mongodb.hadoop.MongoInputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;

/**
 * Import user events
 * 
 * @author jaume
 */
public class ImportCatalogJob extends Job {
    private static final String JOB_NAME = "ImportCatalogJob";

    public ImportCatalogJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(ItemIDIndexObjMapper.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(LongWritable.class);

        this.setReducerClass(ItemIDIndexObjReducer.class);
        this.setOutputKeyClass(IntWritable.class);
        this.setOutputValueClass(LongWritable.class);
    }

    public void configure(String inputUrl, Path outputPath, String queryKey)
            throws IOException {

        BasicDBObject query = new BasicDBObject();
        query.put(queryKey, new BasicDBObject("$ne", "0"));

        MongoConfigUtil.setInputURI(this.conf, inputUrl);
        MongoConfigUtil.setInputKey(this.conf, queryKey);
        MongoConfigUtil.setQuery(this.conf, query);

        this.setInputFormatClass(MongoInputFormat.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);
    }

    public void configure(Path inputPath, Path outputPath) throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);
    }
}
