package es.tid.ps.dynamicprofile;

import com.hadoop.mapreduce.LzoTextInputFormat;
import es.tid.ps.dynamicprofile.categoryextraction.*;

import java.net.URI;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class DynamicProfileMain {
    private static final String COM_SCORE_BASE = "/user/hdfs/comscore/";

    public static void main(String[] args) throws Exception {

        Configuration conf = new Configuration();

        Job job = new Job(conf, "Dynamic profile");
        job.setJobName("dynamicprofile");
        job.setJarByClass(DynamicProfileMain.class);
        job.setInputFormatClass(LzoTextInputFormat.class);
        job.setOutputKeyClass(CompositeKey.class);
        job.setOutputValueClass(CategoryInformation.class);
        job.setOutputFormatClass(TextOutputFormat.class);
        job.setMapperClass(CategoryExtractionMapper.class);
        job.setReducerClass(CategoryExtractionReducer.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        DistributedCache.createSymlink(conf);
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE +
                "cs_terms_in_domain.bcp"),
                job.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE +
                "cs_mmxi_143.bcp.gz"),
                job.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE +
                "pattern_category_mapping_143m.txt"),
                job.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE +
                "cat_subcat_lookup_143m.txt"),
                job.getConfiguration());

        job.waitForCompletion(true);
    }
}
