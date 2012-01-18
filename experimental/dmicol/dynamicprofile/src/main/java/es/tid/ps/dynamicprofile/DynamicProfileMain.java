package es.tid.ps.dynamicprofile;

import es.tid.ps.dynamicprofile.categoryextraction.CategoryExtractionMapper;
import es.tid.ps.dynamicprofile.categoryextraction.CategoryExtractionReducer;
import es.tid.ps.dynamicprofile.categoryextraction.CategoryInformation;
import es.tid.ps.dynamicprofile.categoryextraction.CompositeKey;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class DynamicProfileMain {
    public static void main(String[] args) throws Exception {
        String termsInDomainFlatFileName = args[0];
        String dictionaryFileName = args[1];
        String categoryPatterMappingFileName = args[2];
        String categoryNamesFileName = args[3];
       
        Configuration conf = new Configuration();
        conf.set("termsInDomainFlatFileName", termsInDomainFlatFileName);
        conf.set("dictionaryFileName", dictionaryFileName);
        conf.set("categoryPatterMappingFileName", categoryPatterMappingFileName);
        conf.set("categoryNamesFileName", categoryNamesFileName);
        
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
