package es.tid.bdp.profile;

import java.io.IOException;
import java.util.Calendar;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
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
    private static final int MIN_ARGS = 2;
    private static final int MAX_ARGS = 3;
    
    private static final String INPUT_SERIALIZATION = "input.serialization";
    private static final String PROTOBUF_SERIALIZATION = "protobuf";
    
    private static final String TMP_DIR = "tmp";
    private static final String CATEGORIES_DIR = "categories";
    private static final String PROFILE_DIR = "profile";
    
    private static Logger logger = Logger.getLogger(
            IndividualProfileMain.class);

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length < MIN_ARGS || args.length > MAX_ARGS) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "[-D input.serialization=text|protobuf] "
                    + "weblogs_path psoutput_path [mongo_url]\n"
                    + "\tDefault input serialization is protobuf");
        }
        
        FileSystem fs = FileSystem.get(this.getConf());
        
        Path tmpDir = new Path(getTmpDir());
        if (!fs.mkdirs(tmpDir)) {
            logger.fatal("Could not create " + tmpDir);
        }
        logger.info("Using " + tmpDir + " as temp directory");
        if (!fs.deleteOnExit(tmpDir)) {
            logger.warn("Could not set temp directory for automatic deletion");
        }
        
        Path webLogsPath = new Path(args[0]);
        Path categoriesPath = new Path(
                tmpDir + Path.SEPARATOR + CATEGORIES_DIR);
        Path profilePath = new Path(tmpDir + Path.SEPARATOR + PROFILE_DIR);

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
        if (args.length == MAX_ARGS) {
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

    private static String getTmpDir() {
        final String path = Path.SEPARATOR + TMP_DIR + Path.SEPARATOR +
                "individualprofile" + "_"
                + Calendar.YEAR + Calendar.MONDAY + Calendar.DATE
                + Calendar.HOUR_OF_DAY + Calendar.MINUTE + Calendar.SECOND
                + Calendar.MILLISECOND;
        return path;
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
