package es.tid.ps.profile;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.ps.profile.categoryextraction.CategoryExtractionJob;
import es.tid.ps.profile.export.mongodb.ExporterJob;
import es.tid.ps.profile.userprofile.UserProfileJob;

/**
 * Application entry point. Configures and chains mapreduce jobs.
 *
 * @author dmicol, sortega
 */
public class IndividualProfileMain extends Configured implements Tool {
    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length < 3 || args.length > 4) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "weblogs_path categories_path profile_path [mongo_url]");
        }

        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(args[1]);
        Path profilePath = new Path(args[2]);

        CategoryExtractionJob ceJob = new CategoryExtractionJob(this.getConf());
        ceJob.configure(webLogsPath, categoriesPath);
        if (!ceJob.waitForCompletion(true)) {
            return 1;
        }
        UserProfileJob upJob = new UserProfileJob(this.getConf());
        upJob.configure(categoriesPath, profilePath);
        if (!upJob.waitForCompletion(true)) {
            return 1;
        }

        if (args.length == 4) {
            String mongoUrl = args[3];
            ExporterJob exJob = new ExporterJob(this.getConf());
            exJob.configure(profilePath, mongoUrl);
            if (!exJob.waitForCompletion(true)) {
                return 1;
            }
        }

        return 0;
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
