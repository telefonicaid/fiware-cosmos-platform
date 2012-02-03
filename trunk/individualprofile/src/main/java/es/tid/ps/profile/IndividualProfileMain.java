package es.tid.ps.profile;

import java.io.IOException;
import java.net.URI;

import com.hadoop.mapreduce.LzoTextInputFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.categoryextraction.*;
import es.tid.ps.profile.export.mongodb.ExporterJob;
import es.tid.ps.profile.userprofile.CategoryCount;
import es.tid.ps.profile.userprofile.UserProfile;
import es.tid.ps.profile.userprofile.UserProfileMapper;
import es.tid.ps.profile.userprofile.UserProfileReducer;

/**
 * Application entry point. Configures and chains mapreduce jobs.
 *
 * @author dmicol, sortega
 */
public class IndividualProfileMain extends Configured implements Tool {
    private static final String COM_SCORE_BASE = "/user/hdfs/comscore/";

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length != 4) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "weblogs_path categories_path profile_path mongo_url");
        }

        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(args[1]);
        Path profilePath = new Path(args[2]);
        String mongoUrl = args[3];

        Job ceJob = configureCategoryExtractionJob(webLogsPath, categoriesPath);
        if (!ceJob.waitForCompletion(true)) {
            return 1;
        }
        Job upJob = configureUserProfileJob(categoriesPath, profilePath);
        if (!upJob.waitForCompletion(true)) {
            return 1;
        }
        ExporterJob exJob = new ExporterJob(this.getConf());
        exJob.configure(profilePath, mongoUrl);
        if (!exJob.waitForCompletion(true)) {
            return 1;
        }
        
        return 0;
    }

    private Job configureCategoryExtractionJob(
            Path webLogsPath, Path categoriesPath) throws IOException {
        Configuration baseConf = this.getConf();

        Job job = new Job(baseConf, "CategoryExtraction");
        job.setJarByClass(IndividualProfileMain.class);
        job.setInputFormatClass(LzoTextInputFormat.class);
        job.setMapOutputKeyClass(BinaryKey.class);
        job.setMapOutputValueClass(UserNavigation.class);
        job.setOutputKeyClass(BinaryKey.class);
        job.setOutputValueClass(CategoryInformation.class);
        job.setOutputFormatClass(SequenceFileOutputFormat.class);
        job.setMapperClass(CategoryExtractionMapper.class);
        job.setReducerClass(CategoryExtractionReducer.class);

        FileInputFormat.addInputPath(job, webLogsPath);
        FileOutputFormat.setOutputPath(job, categoriesPath);

        // Distribution of dictionary files by the distributed cache
        DistributedCache.createSymlink(baseConf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_terms_in_domain.bcp"),
                job.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_mmxi.bcp.gz"),
                job.getConfiguration());
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE
                + "patterns_to_categories.txt"), job.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cat_subcat_lookup.txt"),
                job.getConfiguration());

        return job;
    }

    private Job configureUserProfileJob(
            Path categoriesPath, Path profilePath) throws IOException {
        Job job = new Job(this.getConf(), "UserProfile");
        job.setJarByClass(IndividualProfileMain.class);
        job.setInputFormatClass(SequenceFileInputFormat.class);
        job.setMapOutputKeyClass(BinaryKey.class);
        job.setMapOutputValueClass(CategoryCount.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(UserProfile.class);
        job.setOutputFormatClass(SequenceFileOutputFormat.class);
        job.setMapperClass(UserProfileMapper.class);
        job.setReducerClass(UserProfileReducer.class);

        FileInputFormat.addInputPath(job, categoriesPath);
        FileOutputFormat.setOutputPath(job, profilePath);

        return job;
    }

    public static void main(String[] args) {
        try {
            int res = ToolRunner.run(new Configuration(), 
                                    new IndividualProfileMain(), args);
            System.exit(res);
        } catch (Exception ex) {
            ex.printStackTrace(System.err);
            System.exit(1);
        }
    }
}
