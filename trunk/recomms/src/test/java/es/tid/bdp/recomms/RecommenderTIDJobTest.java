package es.tid.bdp.recomms;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.google.common.collect.Maps;
import com.mongodb.BasicDBObject;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.recomms.common.ReadJobConfig;
import es.tid.bdp.recomms.common.TIDProtoBufUtils;
import es.tid.bdp.recomms.data.RecommsProtocol.EntityPref;
import es.tid.bdp.recomms.data.RecommsProtocol.PrefandSimil;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;
import es.tid.bdp.recomms.data.RecommsProtocol.Predictions;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorAndPrefs;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorOrPrefs;
import es.tid.bdp.recomms.extractRecomms.PartialMultiplyMapper;

import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.apache.mahout.cf.taste.impl.TasteTestCase;
import org.apache.mahout.cf.taste.impl.common.FastIDSet;
import org.apache.mahout.cf.taste.impl.recommender.GenericRecommendedItem;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.common.iterator.FileLineIterable;
import org.apache.mahout.math.RandomAccessSparseVector;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.math.hadoop.MathHelper;
import org.easymock.IArgumentMatcher;
import org.easymock.EasyMock;
import org.junit.Test;
import org.bson.BSONObject;

import es.tid.bdp.recomms.importer.ItemIDIndexObjMapper;
import es.tid.bdp.recomms.importer.ItemIDIndexObjReducer;
import es.tid.bdp.recomms.importer.ToEventPrefsMapper;
import es.tid.bdp.recomms.matrixMultiply.endMultiplication.ToVectorAndPrefReducer;
import es.tid.bdp.recomms.matrixMultiply.userVectorSplits.UserVectorSplitterMapper;
import es.tid.bdp.recomms.prepareMatrix.blackList.ToBlackListMapper;
import es.tid.bdp.recomms.prepareMatrix.blackList.ToBlackListReducer;
import es.tid.bdp.recomms.prepareMatrix.userVector.ToUserVectorsCounter;
import es.tid.bdp.recomms.prepareMatrix.userVector.ToUserVectorsReducer;
import es.tid.bdp.recomms.RecommenderTIDJob;

public class RecommenderTIDJobTest extends TasteTestCase {

    /**
     * tests {@link ItemIDIndexObjMapper}
     */
    @Test
    public void testItemIDIndexMapper() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<Object, Object, IntWritable, LongWritable>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(new IntWritable(TasteHadoopUtils.idToIndex(789L)),
                new LongWritable(789L));
        EasyMock.replay(context);

        BasicDBObject event = new BasicDBObject();

        event.put("user_id", "456");
        event.put("item_id", "789");
        event.put("eventValue", "5.0");

        new ItemIDIndexObjMapper().map(new Object(), (BSONObject) event,
                context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link ItemIDIndexObjMapper}
     */
    @Test
    public void testItemIDIndexMapperProtoBuf() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<Object, Object, IntWritable, LongWritable>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(new IntWritable(TasteHadoopUtils.idToIndex(789L)),
                new LongWritable(789L));
        EasyMock.replay(context);

        ProtobufWritable<UserPref> uPref = TIDProtoBufUtils.ProtoBufuserPref(
                56L, 789, 5.0f, 0);

        new ItemIDIndexObjMapper().map(new Object(), uPref, context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link ItemIDIndexObjReducer}
     */
    @Test
    public void testItemIDIndexReducer() throws Exception {
        @SuppressWarnings("unchecked")
        Reducer<IntWritable, LongWritable, IntWritable, LongWritable>.Context context = EasyMock
                .createMock(Reducer.Context.class);

        context.write(new IntWritable(123), new LongWritable(45L));
        EasyMock.replay(context);

        new ItemIDIndexObjReducer().reduce(new IntWritable(123), Arrays.asList(
                new LongWritable(67L), new LongWritable(89L), new LongWritable(
                        45L)), context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link testToEventPrefsMapper}
     */
    @Test
    public void testToEventPrefsMapper() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<Object, Object, LongWritable, ProtobufWritable<UserPref>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        // EasyMock.anyObject();
        BasicDBObject event = new BasicDBObject();

        event.put("user_id", "12");
        event.put("item_id", "4");
        event.put("eventValue", "1.0");
        event.put("device_id", "");
        event.put("event_type", "view");
        event.put("created_at", "");

        BasicDBObject event2 = new BasicDBObject();

        event2.put("user_id", "56");
        event2.put("item_id", "2");
        event2.put("eventValue", "2.0");
        event2.put("device_id", "");
        event2.put("event_type", "purchase");
        event2.put("created_at", "");

        BasicDBObject event3 = new BasicDBObject();

        event3.put("user_id", "56");
        event3.put("item_id", "3");
        event3.put("eventValue", "2.0");
        event3.put("event_type", "click");
        event3.put("created_at", "");
        event3.put("device_id", "");

        BasicDBObject event4 = new BasicDBObject();

        event4.put("user_id", "56");
        event4.put("item_id", "2");
        event4.put("eventValue", "2.0");
        event4.put("device_id", "");
        event4.put("event_type", "recodisplay");
        event4.put("created_at", "");

        context.write(new LongWritable(12L),
                TIDProtoBufUtils.ProtoBufuserPref(12L, 4, 1.0f, 4));
        context.write(new LongWritable(56L),
                TIDProtoBufUtils.ProtoBufuserPref(56L, 3, 2.0f, 3));
        context.write(new LongWritable(56L),
                TIDProtoBufUtils.ProtoBufuserPref(56L, 2, 2.0f, 0));
        context.write(new LongWritable(56L),
                TIDProtoBufUtils.ProtoBufuserPref(56L, 2, 2.0f, 2));

        EasyMock.replay(context);

        ToEventPrefsMapper mapper = new ToEventPrefsMapper();

        mapper.map(new LongWritable(12L), event, context);
        mapper.map(new LongWritable(56L), event2, context);
        mapper.map(new LongWritable(56L), event3, context);
        mapper.map(new LongWritable(56L), event4, context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link testToEventPrefsMapper}
     */
    @Test
    public void testToEventPrefsMapperText() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<Object, Object, LongWritable, ProtobufWritable<UserPref>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(new LongWritable(5978L),
                TIDProtoBufUtils.ProtoBufuserPref(5978L, 208, 1.0f, 1));

        EasyMock.replay(context);

        ToEventPrefsMapper mapper = new ToEventPrefsMapper();

        mapper.map(new LongWritable(5978L), new Text("5978,,rate,208,1,"),
                context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link testToBlackListMapper}
     */
    @Test
    public void testToBlackListMapper() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<LongWritable, ProtobufWritable<UserPref>, LongWritable, LongWritable>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        ProtobufWritable<UserPref> event = new ProtobufWritable<UserPref>();
        ProtobufWritable<UserPref> event2 = new ProtobufWritable<UserPref>();

        event.setConverter(UserPref.class);
        event2.setConverter(UserPref.class);

        event.set(UserPref.newBuilder().setUserID(1).setItemId(1)
                .setEventValue(1.0f).build());

        context.write(new LongWritable(12L), new LongWritable(1));

        EasyMock.replay(context);

        ToBlackListMapper mapper = new ToBlackListMapper();

        mapper.map(new LongWritable(12L), event, context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link testToBlackListReducer}
     */
    @Test
    public void testToBlackListReducer() throws Exception {
        @SuppressWarnings("unchecked")
        Reducer<LongWritable, LongWritable, Text, BSONObject>.Context context = EasyMock
                .createMock(Reducer.Context.class);

        Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 100);
        userVector.set(TasteHadoopUtils.idToIndex(12), 1);

        context.write(new Text("12"),
                (BSONObject) new ToBlackListReducer().toObject(userVector));

        EasyMock.replay(context);

        Collection<LongWritable> varLongWritables = new LinkedList<LongWritable>();
        varLongWritables.add(new LongWritable(12L));

        new ToBlackListReducer().reduce(new LongWritable(12L),
                varLongWritables, context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link ToUserVectorsReducer}
     */
    @Test
    public void testToUserVectorReducer() throws Exception {
        @SuppressWarnings("unchecked")
        Reducer<LongWritable, ProtobufWritable<EntityPref>, LongWritable, VectorWritable>.Context context = EasyMock
                .createMock(Reducer.Context.class);
        Counter userCounters = EasyMock.createMock(Counter.class);

        EasyMock.expect(context.getCounter(ToUserVectorsCounter.USERS))
                .andReturn(userCounters);
        userCounters.increment(1);
        context.write(EasyMock.eq(new LongWritable(12L)), MathHelper
                .vectorMatches(
                        MathHelper.elem(TasteHadoopUtils.idToIndex(34L), 1.0),
                        MathHelper.elem(TasteHadoopUtils.idToIndex(56L), 2.0)));

        EasyMock.replay(context, userCounters);

        ProtobufWritable<EntityPref> EntityPref1 = new ProtobufWritable<EntityPref>();
        ProtobufWritable<EntityPref> EntityPref2 = new ProtobufWritable<EntityPref>();

        EntityPref1.setConverter(EntityPref.class);
        EntityPref2.setConverter(EntityPref.class);

        EntityPref1.set(EntityPref.newBuilder().setItemID(34L).setValue(1.0f)
                .build());
        EntityPref2.set(EntityPref.newBuilder().setItemID(56L).setValue(2.0f)
                .build());

        Collection<ProtobufWritable<EntityPref>> varLongWritables = new LinkedList<ProtobufWritable<EntityPref>>();
        varLongWritables.add(EntityPref1);
        varLongWritables.add(EntityPref2);

        new ToUserVectorsReducer().reduce(new LongWritable(12L),
                varLongWritables, context);

        EasyMock.verify(context, userCounters);
    }

    /**
     * tests {@link ToUserVectorsReducer} using boolean data
     */
    @Test
    public void testToUserVectorReducerWithBooleanData() throws Exception {
        @SuppressWarnings("unchecked")
        Reducer<LongWritable, ProtobufWritable<EntityPref>, LongWritable, VectorWritable>.Context context = EasyMock
                .createMock(Reducer.Context.class);
        Counter userCounters = EasyMock.createMock(Counter.class);

        EasyMock.expect(context.getCounter(ToUserVectorsCounter.USERS))
                .andReturn(userCounters);
        userCounters.increment(1);
        context.write(EasyMock.eq(new LongWritable(12L)), MathHelper
                .vectorMatches(
                        MathHelper.elem(TasteHadoopUtils.idToIndex(34L), 1.0),
                        MathHelper.elem(TasteHadoopUtils.idToIndex(56L), 1.0)));

        EasyMock.replay(context, userCounters);

        ProtobufWritable<EntityPref> EntityPref1 = new ProtobufWritable<EntityPref>();
        ProtobufWritable<EntityPref> EntityPref2 = new ProtobufWritable<EntityPref>();

        EntityPref1.setConverter(EntityPref.class);
        EntityPref2.setConverter(EntityPref.class);

        EntityPref1.set(EntityPref.newBuilder().setItemID(34L).setValue(1.0f)
                .build());
        EntityPref2.set(EntityPref.newBuilder().setItemID(56L).setValue(1.0f)
                .build());

        new ToUserVectorsReducer().reduce(new LongWritable(12L),
                Arrays.asList(EntityPref1, EntityPref2), context);

        EasyMock.verify(context, userCounters);
    }

    /**
     * tests {@link UserVectorSplitterMapper}
     */
    @Test
    public void testUserVectorSplitterMapper() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<LongWritable, VectorWritable, IntWritable, ProtobufWritable<VectorOrPrefs>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(EasyMock.eq(new IntWritable(34)),
                prefOfVectorOrPrefWritableMatches(123L, 0.5f));
        context.write(EasyMock.eq(new IntWritable(56)),
                prefOfVectorOrPrefWritableMatches(123L, 0.7f));

        EasyMock.replay(context);

        UserVectorSplitterMapper mapper = new UserVectorSplitterMapper();
        setField(mapper, "maxPrefsPerUserConsidered", 10);

        RandomAccessSparseVector vector = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);

        vector.set(34, 0.5);
        vector.set(56, 0.7);

        mapper.map(new LongWritable(123L), new VectorWritable(vector), context);

        EasyMock.verify(context);
    }

    /**
     * verifies a preference in a {@link VectorOrPrefWritable}
     */
    private static ProtobufWritable<VectorOrPrefs> prefOfVectorOrPrefWritableMatches(
            final long userID, final float prefValue) {
        EasyMock.reportMatcher(new IArgumentMatcher() {
            @Override
            public boolean matches(Object argument) {
                if (argument instanceof ProtobufWritable<?>) {
                    ProtobufWritable<VectorOrPrefs> pref = (ProtobufWritable<VectorOrPrefs>) argument;
                    return pref.get().getUserID() == userID
                            && pref.get().getValue() == prefValue;
                }
                return false;
            }

            @Override
            public void appendTo(StringBuffer buffer) {
            }
        });
        return null;
    }

    /**
     * tests {@link UserVectorSplitterMapper} in the special case that some
     * userIDs shall be excluded
     */
    @Test
    public void testUserVectorSplitterMapperUserExclusion() throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<LongWritable, VectorWritable, IntWritable, ProtobufWritable<VectorOrPrefs>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(EasyMock.eq(new IntWritable(34)),
                prefOfVectorOrPrefWritableMatches(123L, 0.5f));
        context.write(EasyMock.eq(new IntWritable(56)),
                prefOfVectorOrPrefWritableMatches(123L, 0.7f));

        EasyMock.replay(context);

        FastIDSet usersToRecommendFor = new FastIDSet();
        usersToRecommendFor.add(123L);

        UserVectorSplitterMapper mapper = new UserVectorSplitterMapper();
        setField(mapper, "maxPrefsPerUserConsidered", 10);
        setField(mapper, "usersToRecommendFor", usersToRecommendFor);

        RandomAccessSparseVector vector = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);
        vector.set(34, 0.5);
        vector.set(56, 0.7);

        mapper.map(new LongWritable(123L), new VectorWritable(vector), context);
        mapper.map(new LongWritable(456L), new VectorWritable(vector), context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link UserVectorSplitterMapper} in the special case that the
     * number of preferences to be considered is less than the number of
     * available preferences
     */
    @Test
    public void testUserVectorSplitterMapperOnlySomePrefsConsidered()
            throws Exception {
        @SuppressWarnings("unchecked")
        Mapper<LongWritable, VectorWritable, IntWritable, ProtobufWritable<VectorOrPrefs>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        context.write(EasyMock.eq(new IntWritable(34)),
                prefOfVectorOrPrefWritableMatchesNaN(123L));
        context.write(EasyMock.eq(new IntWritable(56)),
                prefOfVectorOrPrefWritableMatches(123L, 0.7f));

        EasyMock.replay(context);

        UserVectorSplitterMapper mapper = new UserVectorSplitterMapper();
        setField(mapper, "maxPrefsPerUserConsidered", 1);

        RandomAccessSparseVector vector = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);
        vector.set(34, 0.5);
        vector.set(56, 0.7);

        mapper.map(new LongWritable(123L), new VectorWritable(vector), context);

        EasyMock.verify(context);
    }

    /**
     * verifies that a preference value is NaN in a {@link VectorOrPrefWritable}
     */
    private static ProtobufWritable<VectorOrPrefs> prefOfVectorOrPrefWritableMatchesNaN(
            final long userID) {
        EasyMock.reportMatcher(new IArgumentMatcher() {
            @Override
            public boolean matches(Object argument) {
                if (argument instanceof ProtobufWritable<?>) {
                    ProtobufWritable<VectorOrPrefs> pref = (ProtobufWritable<VectorOrPrefs>) argument;
                    return pref.get().getUserID() == userID
                            && Float.isNaN(pref.get().getValue());
                }
                return false;
            }

            @Override
            public void appendTo(StringBuffer buffer) {
            }
        });
        return null;
    }

    /**
     * tests {@link ToVectorAndPrefReducer}
     */
    @Test
    public void testToVectorAndPrefReducer() throws Exception {
        @SuppressWarnings("unchecked")
        Reducer<IntWritable, ProtobufWritable<VectorOrPrefs>, IntWritable, ProtobufWritable<VectorAndPrefs>>.Context context = EasyMock
                .createMock(Reducer.Context.class);

        Vector simil = new RandomAccessSparseVector(Integer.MAX_VALUE, 10);
        simil.set(3, 0.5);
        simil.set(7, 0.8);

        ArrayList<Long> userIds = new ArrayList<Long>();
        userIds.add(123L);
        userIds.add(456L);

        ArrayList<Float> prefs = new ArrayList<Float>();
        prefs.add(1.0f);
        prefs.add(2.0f);

        ProtobufWritable<VectorAndPrefs> vAp = TIDProtoBufUtils.ProtoBufVandP(
                simil, userIds, prefs);
        context.write(new IntWritable(1), vAp);

        EasyMock.replay(context);

        Vector similarityColumn = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 1);
        similarityColumn.set(3, 0.5);
        similarityColumn.set(7, 0.8);
        ProtobufWritable<VectorOrPrefs> itemPref1 = TIDProtoBufUtils
                .ProtoBufVorP(123L, 1.0f);
        ProtobufWritable<VectorOrPrefs> itemPref2 = TIDProtoBufUtils
                .ProtoBufVorP(456L, 2.0f);
        ProtobufWritable<VectorOrPrefs> similarities = TIDProtoBufUtils
                .ProtoBufVorP(similarityColumn);
        new ToVectorAndPrefReducer().reduce(new IntWritable(1),
                Arrays.asList(itemPref1, itemPref2, similarities), context);

        EasyMock.verify(context);
    }

    /**
     * tests {@link PartialMultiplyMapper}
     */
    @Test
    public void testPartialMultiplyMapper() throws Exception {

        Vector similarityColumn = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);
        similarityColumn.set(3, 0.5);
        similarityColumn.set(7, 0.8);

        @SuppressWarnings("unchecked")
        Mapper<IntWritable, ProtobufWritable<VectorAndPrefs>, LongWritable, ProtobufWritable<PrefandSimil>>.Context context = EasyMock
                .createMock(Mapper.Context.class);

        ProtobufWritable<PrefandSimil> one = TIDProtoBufUtils
                .ProtoBufPrefandSimil(1.0f, similarityColumn);
        ProtobufWritable<PrefandSimil> two = TIDProtoBufUtils
                .ProtoBufPrefandSimil(3.0f, similarityColumn);

        context.write(EasyMock.eq(new LongWritable(123L)), EasyMock.eq(one));
        context.write(EasyMock.eq(new LongWritable(456L)), EasyMock.eq(two));

        EasyMock.replay(context);

        ProtobufWritable<VectorAndPrefs> VectorAndPrefs = TIDProtoBufUtils
                .ProtoBufVandP(similarityColumn, Arrays.asList(123L, 456L),
                        Arrays.asList(1.0f, 3.0f));
        new PartialMultiplyMapper().map(new IntWritable(1), VectorAndPrefs,
                context);
        EasyMock.verify(context);
    }

    /**
     * verifies a {@link RecommendedItemsWritable}
     */
    static ProtobufWritable<Predictions> recommendationsMatch(
            final RecommendedItem... items) {
        EasyMock.reportMatcher(new IArgumentMatcher() {
            @Override
            public boolean matches(Object argument) {
                if (argument instanceof Predictions) {
                    Predictions recommendedItemsWritable = (Predictions) argument;
                    List<RecommendedItem> expectedItems = new LinkedList<RecommendedItem>(
                            Arrays.asList(items));
                    return expectedItems.equals(recommendedItemsWritable
                            .getPredictsList());
                }
                return false;
            }

            @Override
            public void appendTo(StringBuffer buffer) {
            }
        });
        return null;
    }

    /**
     * small integration test that runs the full job
     * 
     * As a tribute to
     * http://www.slideshare.net/srowen/collaborative-filtering-at-scale, we
     * recommend people food to animals in this test :)
     * 
     * <pre>
     * 
     *  user-item-matrix
     * 
     *          burger  hotdog  berries  icecream
     *  dog       5       5        2        -
     *  rabbit    2       -        3        5
     *  cow       -       5        -        3
     *  donkey    3       -        -        5
     * 
     * 
     *  item-item-similarity-matrix (tanimoto-coefficient of the item-vectors of the user-item-matrix)
     * 
     *          burger  hotdog  berries icecream
     *  burger    -      0.25    0.66    0.5
     *  hotdog   0.25     -      0.33    0.25
     *  berries  0.66    0.33     -      0.25
     *  icecream 0.5     0.25    0.25     -
     * 
     * 
     *  Prediction(dog, icecream)   = (0.5 * 5 + 0.25 * 5 + 0.25 * 2 ) / (0.5 + 0.25 + 0.25)  ~ 4.3
     *  Prediction(rabbit, hotdog)  = (0.25 * 2 + 0.33 * 3 + 0.25 * 5) / (0.25 + 0.33 + 0.25) ~ 3,3
     *  Prediction(cow, burger)     = (0.25 * 5 + 0.5 * 3) / (0.25 + 0.5)                     ~ 3,7
     *  Prediction(cow, berries)    = (0.33 * 5 + 0.25 * 3) / (0.33 + 0.25)                   ~ 4,1
     *  Prediction(donkey, hotdog)  = (0.25 * 3 + 0.25 * 5) / (0.25 + 0.25)                   ~ 4
     *  Prediction(donkey, berries) = (0.66 * 3 + 0.25 * 5) / (0.66 + 0.25)                   ~ 3,5
     * 
     * </pre>
     */
    @Test
    public void testCompleteJob() throws Exception {

        File inputFile = getTestTempFile("prefs.txt");
        File outputDir = getTestTempDir("output");
        outputDir.delete();
        File tmpDir = getTestTempDir("tmp");

        writeLines(inputFile, "1,,rate,1,5", "1,,rate,2,5", "1,,rate,3,2",
                "2,,rate,1,2", "2,,rate,3,3", "2,,rate,4,5", "3,,rate,2,5",
                "3,,rate,4,3", "4,,rate,1,3", "4,,rate,4,5");

        RecommenderTIDJob recommenderJob = new RecommenderTIDJob();

        Configuration conf = new Configuration();

        conf.setBoolean("mapred.output.compress", false);

        recommenderJob.setConf(conf);
        
        System.out.println(inputFile.getAbsolutePath());
        
        recommenderJob.run(new String[] { "--input",
                inputFile.getAbsolutePath(), "--output",
                outputDir.getAbsolutePath(), "--tempDir",
                tmpDir.getAbsolutePath(), "--propertiesFilename",
                tmpDir.getAbsolutePath(), "--similarityClassname",
                "SIMILARITY_TANIMOTO_COEFFICIENT", "--mongosink",
                Boolean.FALSE.toString(), "--mongosource",
                Boolean.FALSE.toString() });

        Map<Long, List<RecommendedItem>> recommendations = readRecommendations(new File(
                outputDir, "part-r-00000"));

        assertEquals(4, recommendations.size());

        for (Entry<Long, List<RecommendedItem>> entry : recommendations
                .entrySet()) {
            long userID = entry.getKey();
            List<RecommendedItem> items = entry.getValue();
            assertNotNull(items);
            RecommendedItem item1 = items.get(0);

            if (userID == 1L) {
                assertEquals(1, items.size());
                assertEquals(4L, item1.getItemID());
                assertEquals(4.3, item1.getValue(), 0.05);
            } else if (userID == 2L) {
                assertEquals(1, items.size());
                assertEquals(2L, item1.getItemID());
                assertEquals(3.3, item1.getValue(), 0.05);
            } else if (userID == 3L) {
                assertEquals(2, items.size());
                assertEquals(3L, item1.getItemID());
                assertEquals(4.1, item1.getValue(), 0.05);
                RecommendedItem item2 = items.get(1);
                assertEquals(1L, item2.getItemID());
                assertEquals(3.7, item2.getValue(), 0.05);
            } else if (userID == 4L) {
                assertEquals(2, items.size());
                assertEquals(2L, item1.getItemID());
                assertEquals(4.0, item1.getValue(), 0.05);
                RecommendedItem item2 = items.get(1);
                assertEquals(3L, item2.getItemID());
                assertEquals(3.5, item2.getValue(), 0.05);
            }
        }
    }

    /**
     * small integration test for boolean data
     */
    @Test
    public void testCompleteJobBoolean() throws Exception {

        File inputFile = getTestTempFile("prefs.txt");
        File outputDir = getTestTempDir("output");
        outputDir.delete();
        File tmpDir = getTestTempDir("tmp");
        File usersFile = getTestTempFile("users.txt");
        writeLines(usersFile, "3");

        writeLines(inputFile, "1,,rate,1,5", "1,,rate,2,5", "1,,rate,3,2",
                "2,,rate,1,2", "2,,rate,3,3", "2,,rate,4,5", "3,,rate,2,5",
                "3,,rate,4,3", "4,,rate,1,3", "4,,rate,4,5");

        RecommenderTIDJob recommenderJob = new RecommenderTIDJob();

        Configuration conf = new Configuration();
        conf.setBoolean("mapred.output.compress", false);

        recommenderJob.setConf(conf);

        recommenderJob.run(new String[] { "--input",
                inputFile.getAbsolutePath(), "--output",
                outputDir.getAbsolutePath(), "--tempDir",
                tmpDir.getAbsolutePath(), "--similarityClassname",
                "SIMILARITY_COOCCURRENCE", "--boolean", "true",
                "--propertiesFilename", tmpDir.getAbsolutePath(),
                "--mongosink", Boolean.FALSE.toString(), "--mongosource",
                Boolean.FALSE.toString() });

        Map<Long, List<RecommendedItem>> recommendations = readRecommendations(new File(
                outputDir, "part-r-00000"));

        List<RecommendedItem> recommendedToCow = recommendations.get(3L);
        assertEquals(2, recommendedToCow.size());

        RecommendedItem item1 = recommendedToCow.get(0);
        RecommendedItem item2 = recommendedToCow.get(1);

        assertEquals(3L, item1.getItemID());
        assertEquals(1L, item2.getItemID());

        // predicted pref must be the sum of similarities:
        // item1: coocc(burger, hotdog) + coocc(burger, icecream) = 3
        // item2: coocc(berries, hotdog) + coocc(berries, icecream) = 2
        assertEquals(1, item1.getValue(), 0.05);
        assertEquals(1, item2.getValue(), 0.05);
    }

    static Map<Long, List<RecommendedItem>> readRecommendations(File file)
            throws IOException {
        Map<Long, List<RecommendedItem>> recommendations = Maps.newHashMap();
        Iterable<String> lineIterable = new FileLineIterable(file);
        boolean start = false;
        long userID = 0;
        int iIdD = 0;
        float pred = 0.0f;
        List<RecommendedItem> items = new LinkedList<RecommendedItem>();

        for (String line : lineIterable) {
            if (start == false) {
                start = true;
                String[] keyValue = line.split("\t");
                userID = Long.parseLong(keyValue[0]);
                items = new LinkedList<RecommendedItem>();
            } else {
                String[] kv = line.split(":");
                if (kv.length > 1) {
                    if (kv[0].contains("itemID")) {
                        iIdD = Integer.parseInt(kv[1].trim());
                    } else if (kv[0].contains("value")) {
                        pred = Float.parseFloat(kv[1].trim());
                        items.add(new GenericRecommendedItem(iIdD, pred));
                    }
                } else if (line.split("\t").length == 2) {
                    if (start == true) {
                        recommendations.put(userID, items);
                    }
                    String[] keyValue = line.split("\t");
                    userID = Long.parseLong(keyValue[0]);
                    items = new LinkedList<RecommendedItem>();
                }
            }
        }
        recommendations.put(userID, items);
        return recommendations;
    }
    
    /**
     * test Job Read config
     */
    @Test
    public void testReadConfig() throws Exception {
        File inputFile = getTestTempFile("prefs.txt");

        writeLines(inputFile, "MONGO_HOST=localhost", "MONGO_PORT=27017", "MONGO_DB=recommender",
                "MONGO_COLLECTION=recommender", "MONGO_EVENTS=recommender", "MONGO_CATALOG=movie_catalog", "MONGO_BL=blacklist",
                "MONGO_RECOMMS=recommendations", "MAX_RECOMMENDATIONS=10", "TEMPORAL_PATH=/tmp/","OUTPUT_PATH=/tmp/output/","MONGO_MANAGE=true",
                "MONGO_FINAL_REMOVE=true","MONGO_AUTH=false","MONGO_USER_FIELD=user_id","MONGO_ITEM_FIELD=item_id",
                "MONGO_PREFERENCE_FIELD=preference","NEIGHBORHOOD=nearest","USER_TH=0.8","NEIGHBORS_NUMBER=10");
               
        ReadJobConfig.readProperties(inputFile.getAbsolutePath());

        assertEquals(ReadJobConfig.getMongoBL(),"mongodb://localhost:27017/recommender.blacklist");
        assertEquals(ReadJobConfig.getMongoCatalog(),"mongodb://localhost:27017/recommender.movie_catalog");
        assertEquals(ReadJobConfig.getMongoEvents(),"mongodb://localhost:27017/recommender.recommender");
        assertEquals(ReadJobConfig.getMongoRecomms(),"mongodb://localhost:27017/recommender.recommendations");
    }

    /**
     * 
     * @param userID
     *            : Creating recommendations for this user
     * @param items
     *            : list of recommended items
     * @return RecommendedItems
     * @throws IOException
     */
    static List<RecommendedItem> createRecommendations(long userID,
            String[] items) throws IOException {
        Map<Long, List<RecommendedItem>> recommendations = Maps.newHashMap();
        List<RecommendedItem> recomms = new LinkedList<RecommendedItem>();
        for (String reco : items) {
            long itemID = Long.parseLong(reco);
            recomms.add(new GenericRecommendedItem(itemID, 1.0f));
        }
        return recomms;
    }

}