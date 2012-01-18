package es.tid.ps.dynamicprofile;

import es.tid.ps.dynamicprofile.categoryextraction.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class DynamicProfileMain {
    public static void main(String[] args) throws Exception {

        Configuration conf = new Configuration();
        conf.set("termsInDomainFlatFileName", args[0]);
        conf.set("dictionaryFileName", args[1]);
        conf.set("categoryPatterMappingFileName", args[2]);
        conf.set("categoryNamesFileName", args[3]);

        Job job = new Job(conf, "Dynamic profile");
        job.setJobName("dynamicprofile");
        job.setInputFormatClass(TextInputFormat.class);
        job.setOutputKeyClass(CompositeKey.class);
        job.setOutputValueClass(CategoryInformation.class);
        job.setOutputFormatClass(TextOutputFormat.class);
        job.setMapperClass(CategoryExtractionMapper.class);
        job.setReducerClass(CategoryExtractionReducer.class);

        FileInputFormat.addInputPath(job, new Path(args[4]));
        FileOutputFormat.setOutputPath(job, new Path(args[5]));

        job.waitForCompletion(true);
    }
}
