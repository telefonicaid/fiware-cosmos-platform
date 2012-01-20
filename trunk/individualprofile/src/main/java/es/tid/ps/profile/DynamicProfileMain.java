package es.tid.ps.profile;

import es.tid.ps.profile.categoryextraction.CategoryExtractionMapper;
import es.tid.ps.profile.categoryextraction.CategoryExtractionReducer;
import es.tid.ps.profile.categoryextraction.CategoryInformation;
import es.tid.ps.profile.userprofile.CategoryCount;
import es.tid.ps.profile.userprofile.UserProfile;
import es.tid.ps.profile.userprofile.UserProfileMapper;
import es.tid.ps.profile.userprofile.UserProfileReducer;

import com.hadoop.mapreduce.LzoTextInputFormat;
import es.tid.ps.profile.categoryextraction.WebLog;
import java.io.IOException;
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
        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(args[1]);
        Path profilePath = new Path(args[2]);

        Job ceJob = categoryExtractionJob(webLogsPath, categoriesPath);
        if (!ceJob.waitForCompletion(true)) {
            System.exit(1);
        }
        Job upJob = userProfileJob(categoriesPath, profilePath);
        if (!upJob.waitForCompletion(true)) {
            System.exit(1);
        }
    }

    private static Job categoryExtractionJob(Path webLogsPath,
            Path categoriesPath) throws IOException, IllegalStateException {
        Configuration conf = new Configuration();
        Job job = new Job(conf, "CategoryExtraction");
        job.setJarByClass(DynamicProfileMain.class);
        job.setInputFormatClass(LzoTextInputFormat.class);
        job.setMapOutputKeyClass(WebLog.class);
        job.setMapOutputValueClass(NullWritable.class);
        job.setOutputKeyClass(WebLog.class);
        job.setOutputValueClass(CategoryInformation.class);
        job.setOutputFormatClass(SequenceFileOutputFormat.class);
        job.setMapperClass(CategoryExtractionMapper.class);
        job.setReducerClass(CategoryExtractionReducer.class);

        FileInputFormat.addInputPath(job, webLogsPath);
        FileOutputFormat.setOutputPath(job, categoriesPath);

        // Distribution of dictionary files by the distributed cache
        DistributedCache.createSymlink(conf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_terms_in_domain.bcp"),
                job.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_mmxi_143.bcp.gz"),
                job.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE
                + "pattern_category_mapping_143m.txt"), job.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cat_subcat_lookup_143m.txt"),
                job.getConfiguration());

        return job;
    }

    private static Job userProfileJob(Path categoriesPath, Path profilePath)
            throws IOException, IllegalStateException {
        Job job = new Job(new Configuration(), "UserProfile");
        job.setJarByClass(DynamicProfileMain.class);
        job.setInputFormatClass(SequenceFileInputFormat.class);
        job.setMapOutputKeyClass(Text.class);
        job.setMapOutputValueClass(CategoryCount.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(UserProfile.class);
        job.setOutputFormatClass(TextOutputFormat.class);
        job.setMapperClass(UserProfileMapper.class);
        job.setReducerClass(UserProfileReducer.class);

        FileInputFormat.addInputPath(job, categoriesPath);
        FileOutputFormat.setOutputPath(job, profilePath);

        return job;
    }
}
