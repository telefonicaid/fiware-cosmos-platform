package es.tid.bdp.recomms;

import org.apache.commons.lang.ArrayUtils;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.ToolRunner;

import org.apache.mahout.common.AbstractJob;
import org.apache.mahout.common.HadoopUtil;

import org.apache.hadoop.conf.Configuration;

import es.tid.bdp.recomms.exporter.MongoDBExportRecommsJob;
import es.tid.bdp.recomms.extractRecomms.GetRecommsJob;
import es.tid.bdp.recomms.common.ReadJobConfig;

import es.tid.bdp.recomms.matrixMultiply.MultiplyMatrixJob;
import es.tid.bdp.recomms.prepareMatrix.PreparePrefMatrixJob;
import es.tid.bdp.recomms.similarity.RowSimilarityJob;

import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * 
 * Application entry point
 * 
 * @author jaume
 * 
 */
public final class RecommenderTIDJob extends AbstractJob {
    public static final String BOOLEAN_DATA = "booleanData";
    public static boolean mongo = false;

    private static final String RECOMMENDER_PROPERTIES = "/home/jaume/mahout/recommender.properties";

    private static final int MAX_PREF_PER_USER = 1000;
    private static final int MIN_PREF_PER_USER = 1;
    private static final int MAX_PREF_PER_USER_ITEM_SIMILARITY = 1000;
    private static final int MAX_SIMILARITIES_PER_ITEM = 100;
    private static final int NUM_RECOMMENDATIONS = 50;
    private static final String SIMILARITY_CLASSE_NAME = "SIMILARITY_PEARSON_CORRELATION";

    @Override
    public int run(String[] args) throws Exception {

        addOption("input", "input", "Input Events", "input");
        addOption("output", "output", "output Events", "output");
        addOption("propertiesFilename", "f",
                "File containing properties defined for the current Job",
                String.valueOf(RECOMMENDER_PROPERTIES));
        addOption("propertiesFilename", "f",
                "File containing properties defined for the current Job",
                String.valueOf(RECOMMENDER_PROPERTIES));
        addOption("maxPrefsPerUser", "Mpu", "Maximum preferences per user",
                String.valueOf(MAX_PREF_PER_USER));
        addOption("minPrefsPerUser", "mpu", "Minimum preferences per user",
                String.valueOf(MIN_PREF_PER_USER));
        addOption("maxPrefsPerUserInItemSimilarity", "mpis",
                ",maxPrefsPerItemSimilarity",
                String.valueOf(MAX_PREF_PER_USER_ITEM_SIMILARITY));
        addOption("maxSimilaritiesPerItem", "mS", ",maxSimilaritiesPerItem",
                String.valueOf(MAX_SIMILARITIES_PER_ITEM));
        addOption("similarityClassname", "s", "SimilarityType",
                SIMILARITY_CLASSE_NAME);
        addOption("boolean", "b", "boolean data or not",
                Boolean.FALSE.toString());
        addOption("numrecos", "nr",
                "Number of recommendations per user if available",
                String.valueOf(NUM_RECOMMENDATIONS));
        addOption("mongosource", "mr", "Data from mongo",
                Boolean.TRUE.toString());
        addOption("mongosink", "ms", "Data to mongo", Boolean.TRUE.toString());

        Map<String, String> parsedArgs = parseArguments(args);
        if (parsedArgs == null) {
            return -1;
        }

        String similarityClassname = parsedArgs.get("--similarityClassname")
                .toString();

        boolean booleanData = Boolean.parseBoolean(parsedArgs.get("--boolean"));

        int maxPrefsPerUser = Integer.parseInt(parsedArgs
                .get("--maxPrefsPerUser"));
        int minPrefsPerUser = Integer.parseInt(parsedArgs
                .get("--minPrefsPerUser"));
        int maxPrefsPerUserInItemSimilarity = Integer.parseInt(parsedArgs
                .get("--maxPrefsPerUserInItemSimilarity"));
        int maxSimilaritiesPerItem = Integer.parseInt(parsedArgs
                .get("--maxSimilaritiesPerItem"));
        int numRecommendations = Integer.parseInt(parsedArgs.get("--numrecos"));

        boolean mongoInput = Boolean.parseBoolean(parsedArgs
                .get("--mongosource"));
        boolean mongoOutput = Boolean.parseBoolean(parsedArgs
                .get("--mongosink"));

        String inputPath = String.valueOf(parsedArgs.get("--input"));
        String outputPath = String.valueOf(parsedArgs.get("--output"));

        double threshold = RowSimilarityJob.NO_THRESHOLD;

        Path prepPath = getTempPath("preparePreferenceMatrix");
        Path similarityMatrixPath = getTempPath("similarityMatrix");

        Path partialMultiplyPath = getTempPath("partialMultiply");

        AtomicInteger currentPhase = new AtomicInteger();

        int numberOfUsers = -1;

        String[] command_extra = new String[] { null, null };

        Configuration conf = getConf();
        if (mongoInput) {
            ReadJobConfig
                    .readProperties(parsedArgs.get("--propertiesFilename"));

            command_extra = new String[] { "--mongo_in_uri_catalog",
                    ReadJobConfig.getMongoCatalog(), "--mongo_in_uri",
                    ReadJobConfig.getMongoEvents(), "--mongo_out_uri",
                    ReadJobConfig.getMongoRecomms(), "--mongo_black_list",
                    ReadJobConfig.getMongoBL(), "--mongoInput",
                    Boolean.TRUE.toString() };
        } else {
            command_extra = new String[] { "--mongoInput",
                    Boolean.FALSE.toString() };
        }

        if (shouldRunNextPhase(parsedArgs, currentPhase)) {

            String[] common_command = new String[] { "--output",
                    prepPath.toString(), "--input", inputPath,
                    "--maxPrefsPerUser",
                    String.valueOf(maxPrefsPerUserInItemSimilarity),
                    "--minPrefsPerUser", String.valueOf(minPrefsPerUser),
                    "--booleanData", String.valueOf(booleanData), "--tempDir",
                    getTempPath().toString() };

            String[] command = (String[]) ArrayUtils.addAll(common_command,
                    command_extra);
            ToolRunner.run(conf, new PreparePrefMatrixJob(), command);

            numberOfUsers = HadoopUtil.readInt(new Path(prepPath,
                    PreparePrefMatrixJob.NUM_USERS), getConf());
        }
        if (shouldRunNextPhase(parsedArgs, currentPhase)) {
            ToolRunner.run(
                    getConf(),
                    new RowSimilarityJob(),
                    new String[] {
                            "--input",
                            new Path(prepPath,
                                    PreparePrefMatrixJob.RATING_MATRIX)
                                    .toString(), "--output",
                            similarityMatrixPath.toString(),
                            "--numberOfColumns", String.valueOf(numberOfUsers),
                            "--similarityClassname", similarityClassname,
                            "--maxSimilaritiesPerRow",
                            String.valueOf(maxSimilaritiesPerItem),
                            "--excludeSelfSimilarity",
                            String.valueOf(Boolean.TRUE), "--threshold",
                            String.valueOf(threshold), "--tempDir",
                            getTempPath().toString() });
        }

        if (shouldRunNextPhase(parsedArgs, currentPhase)) {

            ToolRunner.run(
                    getConf(),
                    new MultiplyMatrixJob(),
                    new String[] {
                            "--input",
                            similarityMatrixPath.toString(),
                            "--output",
                            partialMultiplyPath.toString(),
                            "--booleanData",
                            String.valueOf(booleanData),
                            "--maxPrefsPerUser",
                            String.valueOf(maxPrefsPerUser),
                            "--userVectors",
                            new Path("preparePreferenceMatrix",
                                    PreparePrefMatrixJob.USER_VECTORS)
                                    .toString(), "--tempDir",
                            getTempPath().toString() });
        }

        // extract out the recommendations
        if (shouldRunNextPhase(parsedArgs, currentPhase)) {
            ToolRunner.run(getConf(), new GetRecommsJob(),
                    new String[] { "--input", partialMultiplyPath.toString(),
                            "--output", getTempPath(outputPath).toString(),
                            "--booleanData", String.valueOf(booleanData),
                            "--mongoOutput", String.valueOf(mongoOutput),
                            "--numRecos", String.valueOf(numRecommendations) });

        }
        // Perform the MongoDB export
        if (mongoOutput == true) {
            MongoDBExportRecommsJob exportToMongo = new MongoDBExportRecommsJob(
                    this.getConf());
            exportToMongo.configure(getTempPath(outputPath),
                    ReadJobConfig.getMongoRecomms());
            if (!exportToMongo.waitForCompletion(true)) {
                return 1;
            }
        }

        return 0;
    }

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new Configuration(), new RecommenderTIDJob(), args);
    }
}
