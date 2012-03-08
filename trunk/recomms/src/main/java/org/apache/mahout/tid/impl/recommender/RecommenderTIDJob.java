package org.apache.mahout.tid.impl.recommender;

import org.apache.commons.lang.ArrayUtils;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.JobContext;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.ToolRunner;

import org.apache.mahout.common.AbstractJob;
import org.apache.mahout.common.HadoopUtil;
import org.apache.mahout.tid.common.ReadJobConfig;
import org.apache.mahout.tid.hadoop.RecommendedItemsWritable;
import org.apache.mahout.tid.hadoop.item.AggregateAndRecommendReducer;
import org.apache.mahout.tid.impl.recommender.RecommenderTIDJob;
import org.apache.mahout.tid.impl.recommender.SimilarityMatrixRowWrapperMapper;
import org.apache.mahout.tid.impl.recommender.ToVectorAndPrefReducer;
import org.apache.mahout.tid.impl.recommender.UserVectorSplitterMapper;
import org.apache.mahout.tid.impl.recommender.VectorAndPrefsWritable;
import org.apache.mahout.tid.impl.recommender.VectorOrPrefWritable;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.PreparePreferenceMatrixJob;
import org.apache.mahout.tid.math.hadoop.similarity.cooccurrence.RowSimilarityJob;

import org.apache.mahout.math.VarIntWritable;
import org.apache.mahout.math.VarLongWritable;

import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import com.mongodb.hadoop.*;

import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 
 * @author jaume
 * 
 */
public final class RecommenderTIDJob extends AbstractJob {

    public static final String BOOLEAN_DATA = "booleanData";
    public static boolean mongo = false;

    private static final String RECOMMENDER_PROPERTIES = "~/mahout/recommender.properties";

    private static final int MAX_PREF_PER_USER = 1000;
    private static final int MIN_PREF_PER_USER = 1;
    private static final int MAX_PREF_PER_USER_ITEM_SIMILARITY = 1000;
    private static final int MAX_SIMILARITIES_PER_ITEM = 100;
    private static final int NUM_RECOMMENDATIONS = 50;
    private static final String SIMILARITY_CLASSE_NAME = "SIMILARITY_PEARSON_CORRELATION";

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.util.Tool#run(java.lang.String[])
     */
    @Override
    public int run(String[] args) throws Exception {

        addInputOption();
        addOutputOption();
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

        Map<String, List<String>> parsedArgs = parseArguments(args, true, true);
        if (parsedArgs == null) {
            return -1;
        }

        String similarityClassname = parsedArgs.get("--similarityClassname")
                .get(0).toString();

        boolean booleanData = Boolean.parseBoolean(parsedArgs.get("--boolean")
                .get(0));

        int maxPrefsPerUser = Integer.parseInt(parsedArgs.get(
                "--maxPrefsPerUser").get(0));
        int minPrefsPerUser = Integer.parseInt(parsedArgs.get(
                "--minPrefsPerUser").get(0));
        int maxPrefsPerUserInItemSimilarity = Integer.parseInt(parsedArgs.get(
                "--maxPrefsPerUserInItemSimilarity").get(0));
        int maxSimilaritiesPerItem = Integer.parseInt(parsedArgs.get(
                "--maxSimilaritiesPerItem").get(0));
        int numRecommendations = Integer.parseInt(parsedArgs.get("--numrecos")
                .get(0));

        boolean mongoInput = Boolean.parseBoolean(parsedArgs.get(
                "--mongosource").get(0));
        boolean mongoOutput = Boolean.parseBoolean(parsedArgs
                .get("--mongosink").get(0));

        double threshold = RowSimilarityJob.NO_THRESHOLD;

        Path prepPath = getTempPath("preparePreferenceMatrix");
        Path similarityMatrixPath = getTempPath("similarityMatrix");
        Path prePartialMultiplyPath1 = getTempPath("prePartialMultiply1");
        Path prePartialMultiplyPath2 = getTempPath("prePartialMultiply2");
        Path partialMultiplyPath = getTempPath("partialMultiply");

        AtomicInteger currentPhase = new AtomicInteger();

        int numberOfUsers = -1;

        String[] command_extra = new String[] { null, null };

        Configuration conf = getConf();

        ReadJobConfig JobConf = new ReadJobConfig(parsedArgs.get(
                "--propertiesFilename").get(0));

        if (mongoInput == true) {
            command_extra = new String[] { "--mongo_in_uri_catalog",
                    JobConf.getMongoCatalog(), "--mongo_in_uri",
                    JobConf.getMongoEvents(), "--mongo_out_uri",
                    JobConf.getMongoRecomms(), "--mongo_black_list",
                    JobConf.getMongoBL(), "--mongoInput",
                    Boolean.TRUE.toString() };
        } else {
            conf.set("mapred.input.dir", getInputPath().toString());
            command_extra = new String[] { "--mongoInput",
                    Boolean.FALSE.toString() };
        }

        if (shouldRunNextPhase(parsedArgs, currentPhase)) {

            String[] common_command = new String[] { "--output",
                    prepPath.toString(), "--maxPrefsPerUser",
                    String.valueOf(maxPrefsPerUserInItemSimilarity),
                    "--minPrefsPerUser", String.valueOf(minPrefsPerUser),
                    "--booleanData", String.valueOf(booleanData), "--tempDir",
                    getTempPath().toString() };

            String[] command = (String[]) ArrayUtils.addAll(common_command,
                    command_extra);
            ToolRunner.run(conf, new PreparePreferenceMatrixJob(), command);

            numberOfUsers = HadoopUtil.readInt(new Path(prepPath,
                    PreparePreferenceMatrixJob.NUM_USERS), getConf());
        }

        if (shouldRunNextPhase(parsedArgs, currentPhase)) {
            // Once DistributedRowMatrix uses the hadoop 0.20 API, we should
            // refactor this call to something like new
            // DistributedRowMatrix(...).rowSimilarity(...)
            // calculate the co-occurrence matrix
            ToolRunner.run(
                    getConf(),
                    new RowSimilarityJob(),
                    new String[] {
                            "--input",
                            new Path(prepPath,
                                    PreparePreferenceMatrixJob.RATING_MATRIX)
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

        // start the multiplication of the co-occurrence matrix by the user
        // vectors
        if (shouldRunNextPhase(parsedArgs, currentPhase)) {

            Job prePartialMultiply1 = prepareJob(similarityMatrixPath,
                    prePartialMultiplyPath1, SequenceFileInputFormat.class,
                    SimilarityMatrixRowWrapperMapper.class,
                    VarIntWritable.class, VectorOrPrefWritable.class,
                    Reducer.class, VarIntWritable.class,
                    VectorOrPrefWritable.class, SequenceFileOutputFormat.class);

            prePartialMultiply1.waitForCompletion(true);
            // continue the multiplication
            Job prePartialMultiply2 = prepareJob(new Path(prepPath,
                    PreparePreferenceMatrixJob.USER_VECTORS),
                    prePartialMultiplyPath2, SequenceFileInputFormat.class,
                    UserVectorSplitterMapper.class, VarIntWritable.class,
                    VectorOrPrefWritable.class, Reducer.class,
                    VarIntWritable.class, VectorOrPrefWritable.class,
                    SequenceFileOutputFormat.class);

            prePartialMultiply2.getConfiguration().setInt(
                    UserVectorSplitterMapper.MAX_PREFS_PER_USER_CONSIDERED,
                    maxPrefsPerUser);
            prePartialMultiply2.waitForCompletion(true);

            // finish the job
            Job partialMultiply = prepareJob(new Path(prePartialMultiplyPath1
                    + "," + prePartialMultiplyPath2), partialMultiplyPath,
                    SequenceFileInputFormat.class, Mapper.class,
                    VarIntWritable.class, VectorOrPrefWritable.class,
                    ToVectorAndPrefReducer.class, VarIntWritable.class,
                    VectorAndPrefsWritable.class,
                    SequenceFileOutputFormat.class);
            setS3SafeCombinedInputPath(partialMultiply, getTempPath(),
                    prePartialMultiplyPath1, prePartialMultiplyPath2);
            partialMultiply.waitForCompletion(true);
        }

        if (shouldRunNextPhase(parsedArgs, currentPhase)) {
            // filter out any users we don't care about
            // convert the user/item pairs to filter if a filterfile has been
            // specified

            String aggregateAndRecommendInput = partialMultiplyPath.toString();

            if (mongoOutput == true) {
                MongoConfigUtil
                        .setInputURI(getConf(), JobConf.getMongoEvents());
                MongoConfigUtil.setOutputURI(getConf(),
                        JobConf.getMongoRecomms());
            }

            // extract out the recommendations

            Job aggregateAndRecommend = prepareJob(new Path(
                    aggregateAndRecommendInput), getTempPath("output"),
                    SequenceFileInputFormat.class, PartialMultiplyMapper.class,
                    VarLongWritable.class,
                    PrefAndSimilarityColumnWritable.class,
                    AggregateAndRecommendReducer.class, VarLongWritable.class,
                    RecommendedItemsWritable.class,
                    mongoOutput == true ? MongoOutputFormat.class
                            : TextOutputFormat.class);

            Configuration aggregateAndRecommendConf = aggregateAndRecommend
                    .getConfiguration();

            if (mongoOutput == false)
                aggregateAndRecommendConf.set("mapred.output.dir",
                        getOutputPath().toString());

            setIOSort(aggregateAndRecommend);
            aggregateAndRecommendConf.set(
                    AggregateAndRecommendReducer.ITEMID_INDEX_PATH, new Path(
                            prepPath, PreparePreferenceMatrixJob.ITEMID_INDEX)
                            .toString());

            aggregateAndRecommendConf.setInt(
                    AggregateAndRecommendReducer.NUM_RECOMMENDATIONS,
                    numRecommendations);
            aggregateAndRecommendConf.setBoolean(BOOLEAN_DATA, booleanData);
            aggregateAndRecommend.waitForCompletion(true);

        }

        return 0;
    }

    /**
     * 
     * @param job
     */
    private static void setIOSort(JobContext job) {
        Configuration conf = job.getConfiguration();
        conf.setInt("io.sort.factor", 100);
        String javaOpts = conf.get("mapred.map.child.java.opts"); // new arg
                                                                  // name
        if (javaOpts == null) {
            javaOpts = conf.get("mapred.child.java.opts"); // old arg name
        }
        int assumedHeapSize = 512;
        if (javaOpts != null) {
            Matcher m = Pattern.compile("-Xmx([0-9]+)([mMgG])").matcher(
                    javaOpts);
            if (m.find()) {
                assumedHeapSize = Integer.parseInt(m.group(1));
                String megabyteOrGigabyte = m.group(2);
                if ("g".equalsIgnoreCase(megabyteOrGigabyte)) {
                    assumedHeapSize *= 1024;
                }
            }
        }
        // Cap this at 1024MB now; see
        // https://issues.apache.org/jira/browse/MAPREDUCE-2308
        conf.setInt("io.sort.mb", Math.min(assumedHeapSize / 2, 1024));
        // For some reason the Merger doesn't report status for a long time;
        // increase
        // timeout when running these jobs
        conf.setInt("mapred.task.timeout", 60 * 60 * 1000);

    }

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new Configuration(), new RecommenderTIDJob(), args);
    }
}
