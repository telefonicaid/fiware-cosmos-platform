package es.tid.bdp.profile;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.bdp.profile.categoryextraction.CategoryExtractionJob;
import es.tid.bdp.profile.export.mongodb.MongoDBExporterJob;
import es.tid.bdp.profile.export.ps.PSExporterJob;
import es.tid.bdp.profile.userprofile.UserProfileJob;

/**
 * Application entry point. Configures and chains mapreduce jobs.
 *
 * @author dmicol, sortega
 */
public class IndividualProfileMain extends Configured implements Tool {
    public static final String INPUT_SERIALIZATION = "input.serialization";
    public static final String PROTOBUF_SERIALIZATION = "protobuf";

    private static final String TMP_DIR = "/tmp/individualprofile";
    private static final String CATEGORIES_PATH = TMP_DIR + "/categories";
    private static final String PROFILE_PATH = TMP_DIR + "profile";
    
    private static Logger logger = Logger.getLogger(
            IndividualProfileMain.class);

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length < 2 || args.length > 3) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "[-D input.serialization=text|protobuf] "
                    + "weblogs_path psoutput_path [mongo_url]\n"
                    + "\tDefault input serialization is protobuf");
        }

        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(CATEGORIES_PATH);
        Path profilePath = new Path(PROFILE_PATH);

        CategoryExtractionJob ceJob = new CategoryExtractionJob(this.getConf());
        if (this.getConf().get(INPUT_SERIALIZATION, PROTOBUF_SERIALIZATION)
                .equals(PROTOBUF_SERIALIZATION)) {
            logger.info("Protobuf input");
            ceJob.configureProtobufInput();
        } else {
            logger.info("Text input");
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
        
        String psOutputFile = args[1];
        PSExporterJob exPsJob = new PSExporterJob(this.getConf());
        exPsJob.configure(profilePath, new Path(psOutputFile));
        if (!exPsJob.waitForCompletion(true)) {
            return 1;
        }
        
        // Perform the MongoDB export.
        if (args.length == 3) {
            String mongoUrl = args[2];
            MongoDBExporterJob exMongoJob = new MongoDBExporterJob(
                    this.getConf());
            exMongoJob.configure(profilePath, mongoUrl);
            if (!exMongoJob.waitForCompletion(true)) {
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
