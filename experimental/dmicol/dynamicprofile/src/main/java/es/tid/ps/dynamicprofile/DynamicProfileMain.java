package es.tid.ps.dynamicprofile;

import es.tid.ps.dynamicprofile.categoryextraction.CategoryExtractionMapper;
import es.tid.ps.dynamicprofile.categoryextraction.CategoryExtractionReducer;
import es.tid.ps.dynamicprofile.categoryextraction.CategoryInformation;
import es.tid.ps.dynamicprofile.categoryextraction.CompositeKey;
import es.tid.ps.dynamicprofile.userprofile.CategoryCount;
import es.tid.ps.dynamicprofile.userprofile.UserProfile;
import es.tid.ps.dynamicprofile.userprofile.UserProfileMapper;
import es.tid.ps.dynamicprofile.userprofile.UserProfileReducer;

import com.hadoop.mapreduce.LzoTextInputFormat;
import java.net.URI;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class DynamicProfileMain {
    private static final String COM_SCORE_BASE = "/user/hdfs/comscore/";

    public static void main(String[] args) throws Exception {        
        Configuration conf = new Configuration();
        Job ceJob = new Job(conf, "CategoryExtraction");
        ceJob.setJarByClass(DynamicProfileMain.class);
        ceJob.setInputFormatClass(LzoTextInputFormat.class);
        ceJob.setMapOutputKeyClass(CompositeKey.class);
        ceJob.setMapOutputValueClass(NullWritable.class);
        ceJob.setOutputKeyClass(CompositeKey.class);
        ceJob.setOutputValueClass(CategoryInformation.class);
        ceJob.setOutputFormatClass(SequenceFileOutputFormat.class);
        ceJob.setMapperClass(CategoryExtractionMapper.class);
        ceJob.setReducerClass(CategoryExtractionReducer.class);
        FileInputFormat.addInputPath(ceJob, new Path(args[0]));
        FileOutputFormat.setOutputPath(ceJob, new Path(args[1]));
        
        DistributedCache.createSymlink(conf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_terms_in_domain.bcp"),
                ceJob.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_mmxi_143.bcp.gz"),
                ceJob.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE
                + "pattern_category_mapping_143m.txt"), ceJob.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cat_subcat_lookup_143m.txt"),
                ceJob.getConfiguration());
        
        if (!ceJob.waitForCompletion(true)) {
            System.exit(1);
        }
        
        Job upJob = new Job(conf, "UserProfile");
        upJob.setJarByClass(DynamicProfileMain.class);
        upJob.setInputFormatClass(SequenceFileInputFormat.class);
        upJob.setMapOutputKeyClass(Text.class);
        upJob.setMapOutputValueClass(CategoryCount.class);
        upJob.setOutputKeyClass(Text.class);
        upJob.setOutputValueClass(UserProfile.class);
        upJob.setOutputFormatClass(TextOutputFormat.class);
        upJob.setMapperClass(UserProfileMapper.class);
        upJob.setReducerClass(UserProfileReducer.class);
        FileInputFormat.addInputPath(upJob, new Path(args[1]));
        FileOutputFormat.setOutputPath(upJob, new Path(args[2]));
        if (!upJob.waitForCompletion(true)) {
            System.exit(1);
        }
    }
}
