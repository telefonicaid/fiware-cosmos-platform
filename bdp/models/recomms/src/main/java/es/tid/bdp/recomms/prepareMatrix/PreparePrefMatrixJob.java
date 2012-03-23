package es.tid.bdp.recomms.prepareMatrix;

import org.apache.hadoop.util.ToolRunner;

import org.apache.mahout.common.AbstractJob;
import org.apache.mahout.common.HadoopUtil;

import es.tid.bdp.recomms.importer.ImportCatalogJob;
import es.tid.bdp.recomms.importer.ImportEventsJob;

import es.tid.bdp.recomms.RecommenderTIDJob;
import es.tid.bdp.recomms.prepareMatrix.PreparePrefMatrixJob;
import es.tid.bdp.recomms.prepareMatrix.blackList.ComputeBlackListJob;
import es.tid.bdp.recomms.prepareMatrix.userVector.ComputeUserVectorsJob;
import es.tid.bdp.recomms.prepareMatrix.userVector.ToUserVectorsCounter;

import java.util.Map;

public class PreparePrefMatrixJob extends AbstractJob {
    public static final String NUM_USERS = "numUsers.bin";
    public static final String ITEMID_INDEX = "itemIDIndex";
    public static final String USER_VECTORS = "userVectors";
    public static final String RATING_MATRIX = "ratingMatrix";
    public static final String BLACK_LIST = "blacklist";
    public static final String EVENTS = "events";
    public static final String EVENTSW = "eventsw";

    public static String mongoIn;
    public static String mongoOut;
    public static String mongoCatalog;
    public static String mongoBL;

    public static boolean mongoInput = false;

    private static final int DEFAULT_MIN_PREFS_PER_USER = 1;

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new PreparePrefMatrixJob(), args);
    }

    @Override
    public int run(String[] args) throws Exception {

        addInputOption();
        addOutputOption();
        addOption("maxPrefsPerUser", "maxPrefs",
                "max number of preferences to consider per user");
        addOption("minPrefsPerUser", "minPrefs",
                "less preferences than this ignored" + "(default: "
                        + DEFAULT_MIN_PREFS_PER_USER + ')',
                String.valueOf(DEFAULT_MIN_PREFS_PER_USER));
        addOption("booleanData", "bool", "Treat input as without pref values",
                Boolean.FALSE.toString());
        addOption("mongo_in_uri", "mongoInUri", "Mongo INPUT URI",
                Boolean.FALSE.toString());
        addOption("mongo_out_uri", "mongoOutUri", "Mongo OUTPUT URI",
                Boolean.FALSE.toString());
        addOption("mongo_in_uri_catalog", "mongoInUriCatalog",
                "Mongo INPUT CATALOG  URI", Boolean.FALSE.toString());
        addOption("mongo_black_list", "mongoBLUri", "Mongo BlackList  URI",
                Boolean.FALSE.toString());
        addOption("mongoInput", "mongoInput", "Mongo Input",
                Boolean.FALSE.toString());
        addOption("tempDir", "t", "temporal folder", Boolean.FALSE.toString());
        addOption("ratingShift", "rs", "shift ratings by this value", "0.0");

        Map<String, String> parsedArgs = parseArguments(args);
        if (parsedArgs == null) {
            return -1;
        }
        int minPrefsPerUser = Integer.parseInt(parsedArgs
                .get("--minPrefsPerUser"));
        boolean booleanData = Boolean.valueOf(parsedArgs.get("--booleanData"));
        boolean mongoInput = Boolean.valueOf(parsedArgs.get("--mongoInput"));

        mongoIn = parsedArgs.get("--mongo_in_uri");
        mongoOut = parsedArgs.get("--mongo_out_uri");
        mongoCatalog = parsedArgs.get("--mongo_in_uri_catalog");
        mongoBL = parsedArgs.get("--mongo_black_list");

        ImportEventsJob importEvents = new ImportEventsJob(this.getConf());
        ImportCatalogJob importCatalog = new ImportCatalogJob(this.getConf());

        if (mongoInput) {
            importEvents.configure(mongoIn, getOutputPath(EVENTSW), "user_id");
            importCatalog.configure(mongoIn, getOutputPath(ITEMID_INDEX),
                    "item_id");

        } else {
            importEvents.configure(getInputPath(), getOutputPath(EVENTSW));
            importCatalog.configure(getOutputPath(EVENTSW),
                    getOutputPath(ITEMID_INDEX));

        }
        if (!importEvents.waitForCompletion(true)) {
            return 1;
        }
        if (!importCatalog.waitForCompletion(true)) {
            return 1;
        }
        if (mongoInput) {
            ComputeBlackListJob computeBList = new ComputeBlackListJob(
                    this.getConf());
            computeBList.configure(getOutputPath(EVENTSW), mongoBL);
            if (!computeBList.waitForCompletion(true)) {
                return 1;
            }
        }

        ComputeUserVectorsJob computeUvectors = new ComputeUserVectorsJob(
                this.getConf());
        computeUvectors.configure(getOutputPath(EVENTSW),
                getOutputPath(USER_VECTORS), minPrefsPerUser);
        computeUvectors.getConfiguration().setBoolean(
                RecommenderTIDJob.BOOLEAN_DATA, booleanData);
        if (!computeUvectors.waitForCompletion(true)) {
            return 1;
        }

        int numberOfUsers = (int) computeUvectors.getCounters()
                .findCounter(ToUserVectorsCounter.USERS).getValue();

        HadoopUtil.writeInt(numberOfUsers, getOutputPath(NUM_USERS), getConf());

        ComputeItemVectorsJob toItemVectors = new ComputeItemVectorsJob(
                this.getConf());
        toItemVectors.configure(getOutputPath(USER_VECTORS),
                getOutputPath(RATING_MATRIX));
        toItemVectors.getConfiguration().setBoolean(
                RecommenderTIDJob.BOOLEAN_DATA, booleanData);
        if (parsedArgs.containsKey("--maxPrefsPerUser")) {
            int samplingSize = Integer.parseInt(parsedArgs
                    .get("--maxPrefsPerUser"));
            toItemVectors.getConfiguration().setInt(
                    ToItemVectorsMapper.SAMPLE_SIZE, samplingSize);
        }
        if (!toItemVectors.waitForCompletion(true)) {
            return 1;
        }

        return 0;
    }
}
