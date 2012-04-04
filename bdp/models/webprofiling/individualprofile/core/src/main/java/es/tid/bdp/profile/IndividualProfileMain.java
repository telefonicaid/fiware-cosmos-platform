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
    private static final Logger LOG = Logger.getLogger(
            IndividualProfileMain.class);
    private static final int MIN_ARGS = 2;
    private static final int MAX_ARGS = 3;

    private static final String INPUT_SERIALIZATION = "input.serialization";
    private static final String PROTOBUF_SERIALIZATION = "protobuf";
    private static final String TMP_DIR = "tmp";
    private static final String CATEGORIES_DIR = "categories";
    private static final String PROFILE_DIR = "profile";

    private Path categoriesPath;
    private Path profilePath;
    private Path webLogsPath;

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length < MIN_ARGS || args.length > MAX_ARGS) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "[-D input.serialization=text|protobuf] "
                    + "weblogs_path psoutput_path [mongo_url]\n"
                    + "\tDefault input serialization is protobuf");
        }

        this.initPaths(args[0]);

        CategoryExtractionJob ceJob = new CategoryExtractionJob(this.getConf());
        if (this.getConf().get(INPUT_SERIALIZATION, PROTOBUF_SERIALIZATION)
                .equals(PROTOBUF_SERIALIZATION)) {
            LOG.info("Protobuf input");
            ceJob.configureProtobufInput();
        } else {
            LOG.info("Text input");
            ceJob.configureTextInput();
        }
        ceJob.configurePaths(this.webLogsPath, this.categoriesPath);
        if (!ceJob.waitForCompletion(true)) {
            return 1;
        }

        UserProfileJob upJob = new UserProfileJob(this.getConf());
        upJob.configure(this.categoriesPath, this.profilePath);
        if (!upJob.waitForCompletion(true)) {
            return 1;
        }

        String psOutputFile = args[1];
        PSExporterJob exPsJob = new PSExporterJob(this.getConf());
        exPsJob.configure(this.profilePath, new Path(psOutputFile));
        if (!exPsJob.waitForCompletion(true)) {
            return 1;
        }

        // Perform the MongoDB export.
        if (args.length == MAX_ARGS) {
            String mongoUrl = args[2];
            MongoDBExporterJob exMongoJob = new MongoDBExporterJob(
                    this.getConf());
            exMongoJob.configure(this.profilePath, mongoUrl);
            if (!exMongoJob.waitForCompletion(true)) {
                return 1;
            }
        }

        return 0;
    }

    private void initPaths(String inputPath) throws IOException {
        FileSystem fs = FileSystem.get(this.getConf());
        Path tmpDir = new Path(getTmpDir());
        if (!fs.mkdirs(tmpDir)) {
            LOG.fatal("Could not create " + tmpDir);
            throw new IOException("Failed to create temp directory");
        }
        LOG.info("Using " + tmpDir + " as temp directory");
        if (!fs.deleteOnExit(tmpDir)) {
            LOG.warn("Could not set temp directory for automatic deletion");
        }
        this.webLogsPath = new Path(inputPath);
        this.categoriesPath = new Path(tmpDir + Path.SEPARATOR +
                                       CATEGORIES_DIR);
        this.profilePath = new Path(tmpDir + Path.SEPARATOR + PROFILE_DIR);
    }

    private static String getTmpDir() {
        Calendar now = Calendar.getInstance();
        StringBuilder buffer = new StringBuilder();
        buffer.append(Path.SEPARATOR)
                .append(TMP_DIR)
                .append(Path.SEPARATOR)
                .append("individualprofile_")
                .append(now.get(Calendar.YEAR))
                .append(now.get(Calendar.MONTH))
                .append(now.get(Calendar.DATE))
                .append(now.get(Calendar.HOUR_OF_DAY))
                .append(now.get(Calendar.MINUTE))
                .append(now.get(Calendar.SECOND))
                .append(now.get(Calendar.MILLISECOND));
        return buffer.toString();
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(),
                                 new IndividualProfileMain(), args);
        if (res != 0) {
            throw new Exception("Process failed");
        }
    }
}
