package es.tid.bdp.profile;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.bdp.profile.categoryextraction.CategoryExtractionJob;
import es.tid.bdp.profile.export.mongodb.ExporterJob;
import es.tid.bdp.profile.userprofile.UserProfileJob;

/**
 * Application entry point. Configures and chains mapreduce jobs.
 *
 * @author dmicol, sortega
 */
public class IndividualProfileMain extends Configured implements Tool {
    private static final String TMP_DIR = "/tmp/individualprofile";
    private static final String CATEGORIES_PATH = TMP_DIR + "/categories";
    private static final String PROFILE_PATH = TMP_DIR + "profile";

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length < 1 || args.length > 2) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "[-D input.serialization=text|protobuf] "
                    + "weblogs_path [mongo_url]");
        }

        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(CATEGORIES_PATH);
        Path profilePath = new Path(PROFILE_PATH);

        CategoryExtractionJob ceJob = new CategoryExtractionJob(this.getConf());
        if (this.getConf().get("input.serialization", "protobuf").equals("protobuf")) {
            System.out.println("Protobuf input");
            ceJob.configureProtobufInput();
        } else {
            System.out.println("Text input");
            ceJob.configureTextInput();
        }
        ceJob.configurePaths(webLogsPath, categoriesPath);
        if (!ceJob.waitForCompletion(true)) {
            return 1;
        }

        UserProfileJob upJob = new UserProfileJob(this.getConf());
        upJob.configure(categoriesPath, profilePath);
        if (!upJob.waitForCompletion(true)) {
            return 1;
        }

        // Perform the MongoDB export.
        if (args.length == 2) {
            String mongoUrl = args[1];
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
