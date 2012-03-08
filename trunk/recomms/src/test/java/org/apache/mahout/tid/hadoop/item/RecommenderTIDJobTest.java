package org.apache.mahout.tid.hadoop.item;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import com.google.common.collect.Maps;
import com.mongodb.BasicDBObject;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.apache.mahout.cf.taste.hadoop.EntityPrefWritable;
import org.apache.mahout.tid.hadoop.ToEventPrefsMapper;
import org.apache.mahout.tid.hadoop.EventsWritable;
import org.apache.mahout.tid.hadoop.RecommsWritable;
import org.apache.mahout.tid.hadoop.ToElementPrefsMapper;
import org.apache.mahout.tid.hadoop.item.ItemIDIndexObjMapper;
import org.apache.mahout.tid.hadoop.item.ItemIDIndexObjReducer;
import org.apache.mahout.tid.hadoop.item.ToUserVectorsReducer;
import org.apache.mahout.tid.impl.recommender.PartialMultiplyMapper;
import org.apache.mahout.tid.impl.recommender.PrefAndSimilarityColumnWritable;
import org.apache.mahout.tid.impl.recommender.RecommenderTIDJob;
import org.apache.mahout.tid.impl.recommender.SimilarityMatrixRowWrapperMapper;
import org.apache.mahout.tid.impl.recommender.ToVectorAndPrefReducer;

import org.apache.mahout.tid.impl.recommender.UserVectorSplitterMapper;
import org.apache.mahout.tid.impl.recommender.VectorAndPrefsWritable;
import org.apache.mahout.tid.impl.recommender.VectorOrPrefWritable;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToBlackListMapper;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToBlackListReducer;
import org.apache.mahout.cf.taste.impl.TasteTestCase;
import org.apache.mahout.cf.taste.impl.common.FastIDSet;
import org.apache.mahout.cf.taste.impl.recommender.GenericRecommendedItem;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.common.iterator.FileLineIterable;
import org.apache.mahout.math.RandomAccessSparseVector;
import org.apache.mahout.math.VarIntWritable;
import org.apache.mahout.math.VarLongWritable;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.math.hadoop.MathHelper;
import org.easymock.IArgumentMatcher;
import org.easymock.EasyMock;
import org.junit.Test;
import org.bson.BSONObject;

import com.google.common.io.Resources;

public class RecommenderTIDJobTest extends TasteTestCase {

	/**
	 * tests {@link ItemIDIndexObjMapper}
	 */
	@Test
	public void testItemIDIndexMapper() throws Exception {
		Mapper<Object, BSONObject, VarIntWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(new VarIntWritable(TasteHadoopUtils.idToIndex(789L)),
				new VarLongWritable(789L));
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
	 * tests {@link ItemIDIndexObjReducer}
	 */
	@Test
	public void testItemIDIndexReducer() throws Exception {
		Reducer<VarIntWritable, VarLongWritable, VarIntWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Reducer.Context.class);

		context.write(new VarIntWritable(123), new VarLongWritable(45L));
		EasyMock.replay(context);

		new ItemIDIndexObjReducer().reduce(new VarIntWritable(123), Arrays
				.asList(new VarLongWritable(67L), new VarLongWritable(89L),
						new VarLongWritable(45L)), context);

		EasyMock.verify(context);
	}

	/**
	 * tests {@link ToItemPrefsMapper}
	 */
	@Test
	public void testToItemPrefsMapper() throws Exception {
		Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(new VarLongWritable(12L), new EntityPrefWritable(34L,
				1.0f));
		context.write(new VarLongWritable(56L), new EntityPrefWritable(78L,
				2.0f));
		EasyMock.replay(context);

		ToElementPrefsMapper mapper = new ToElementPrefsMapper();

		mapper.map(new VarLongWritable(12L),
				new EventsWritable(34L, 12L, 1.0f), context);
		mapper.map(new VarLongWritable(56L),
				new EventsWritable(78L, 56L, 2.0f), context);
		EasyMock.verify(context);
	}

	/**
	 * tests {@link testToEventPrefsMapper}
	 */
	@Test
	public void testToEventPrefsMapper() throws Exception {
		Mapper<Object, Object, VarLongWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		BasicDBObject event = new BasicDBObject();

		event.put("user_id", "12");
		event.put("item_id", "34");
		event.put("eventValue", "1.0");
		event.put("event_type", "view");

		BasicDBObject event2 = new BasicDBObject();

		event2.put("user_id", "56");
		event2.put("item_id", "78");
		event2.put("eventValue", "2.0");
		event2.put("event_type", "purchase");

		BasicDBObject event3 = new BasicDBObject();

		event3.put("user_id", "56");
		event3.put("item_id", "78");
		event3.put("eventValue", "2.0");
		event3.put("event_type", "click");

		BasicDBObject event4 = new BasicDBObject();

		event4.put("user_id", "56");
		event4.put("item_id", "78");
		event4.put("eventValue", "2.0");
		event4.put("event_type", "recodisplay");

		context.write(new VarLongWritable(12L),
				new EventsWritable(34L, 4, 1.0f));
		context.write(new VarLongWritable(56L),
				new EventsWritable(78L, 0, 2.0f));
		context.write(new VarLongWritable(56L),
				new EventsWritable(78L, 3, 2.0f));
		context.write(new VarLongWritable(56L),
				new EventsWritable(78L, 2, 2.0f));
		EasyMock.replay(context);

		ToEventPrefsMapper mapper = new ToEventPrefsMapper();
		mapper.map((Object) new VarLongWritable(12L), event, context);
		mapper.map((Object) new VarLongWritable(56L), event2, context);
		mapper.map((Object) new VarLongWritable(56L), event3, context);
		mapper.map((Object) new VarLongWritable(56L), event4, context);
		EasyMock.verify(context);
	}

	/**
	 * tests {@link testToBlackListMapper}
	 */
	@Test
	public void testToBlackListMapper() throws Exception {
		Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		EventsWritable event = new EventsWritable();
		EventsWritable event2 = new EventsWritable();
		event.set(1, 1.0f, 1);
		event2.set(1, 1.0f, 2);

		context.write(new VarLongWritable(12L), new VarLongWritable(1));

		EasyMock.replay(context);

		ToBlackListMapper mapper = new ToBlackListMapper();

		mapper.map(new VarLongWritable(12L), event, context);
		mapper.map(new VarLongWritable(12L), event2, context);

		EasyMock.verify(context);
	}

	/**
	 * tests {@link testToBlackListReducer}
	 */
	@Test
	public void testToBlackListReducer() throws Exception {
		Reducer<VarLongWritable, VarLongWritable, Text, BSONObject>.Context context = EasyMock
				.createMock(Reducer.Context.class);

		Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 100);
		userVector.set(TasteHadoopUtils.idToIndex(1), 1);

		VectorWritable vw = new VectorWritable(userVector);
		vw.setWritesLaxPrecision(true);

		context.write(new Text("12"),
				(BSONObject) new ToBlackListReducer().toObject(vw));

		EasyMock.replay(context);

		Collection<VarLongWritable> varLongWritables = new LinkedList<VarLongWritable>();
		varLongWritables.add(new EntityPrefWritable(1L, 1.0f));

		new ToBlackListReducer().reduce(new VarLongWritable(12L),
				varLongWritables, context);

		EasyMock.verify(context);
	}

	@Test
	public void testEntityPrefWritable() throws Exception {
		EntityPrefWritable entity = new EntityPrefWritable();
		entity.set(1, 1.0f);

		assertEquals(entity.getID(), 1);
		EntityPrefWritable entity2 = new EntityPrefWritable(entity);
		assertEquals(entity2.get(), 1);
		assertEquals(entity2.toString(), "1\t1.0");
		EntityPrefWritable entity3 = entity.clone();
		assertEquals(entity3.get(), 1);
	}

	@Test
	public void testEventsWritable() throws Exception {
		EventsWritable eventW = new EventsWritable();
		eventW.set(1, 1.0f, 1);

		assertEquals(eventW.getID(), 1);
		EventsWritable eventW2 = new EventsWritable(eventW);
		assertEquals(eventW2.get(), 1);
		assertEquals(eventW2.toString(), "1\t1.0");
		assertEquals(eventW2.getEventType(), 1);
		assertEquals(eventW2.getisBoolean(), false);
		assertEquals(eventW2.equals(eventW), true);

		EventsWritable eventW3 = eventW.clone();
		assertEquals(eventW3.get(), 1);

		EventsWritable eventW4 = new EventsWritable(1, 1);
		assertEquals(eventW4.getisBoolean(), true);
	}

	/**
	 * tests {@link ToItemPrefsMapper} using boolean data
	 */
	@Test
	public void testToItemPrefsMapperBooleanData() throws Exception {
		Mapper<VarLongWritable, EventsWritable, VarLongWritable, VarLongWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(new VarLongWritable(12L), new VarLongWritable(34L));
		context.write(new VarLongWritable(56L), new VarLongWritable(78L));
		EasyMock.replay(context);

		ToElementPrefsMapper mapper = new ToElementPrefsMapper();
		setField(mapper, "booleanData", true);
		mapper.map(new VarLongWritable(12L), new EventsWritable(34L, 12L),
				context);
		mapper.map(new VarLongWritable(56L), new EventsWritable(78L, 56L),
				context);

		EasyMock.verify(context);
	}

	/**
	 * tests {@link ToUserVectorsReducer}
	 */
	@Test
	public void testToUserVectorReducer() throws Exception {
		Reducer<VarLongWritable, VarLongWritable, VarLongWritable, VectorWritable>.Context context = EasyMock
				.createMock(Reducer.Context.class);
		Counter userCounters = EasyMock.createMock(Counter.class);

		EasyMock.expect(context.getCounter(ToUserVectorsReducer.Counters.USERS))
				.andReturn(userCounters);
		userCounters.increment(1);
		context.write(EasyMock.eq(new VarLongWritable(12L)), MathHelper
				.vectorMatches(
						MathHelper.elem(TasteHadoopUtils.idToIndex(34L), 1.0),
						MathHelper.elem(TasteHadoopUtils.idToIndex(56L), 2.0)));

		EasyMock.replay(context, userCounters);

		Collection<VarLongWritable> varLongWritables = new LinkedList<VarLongWritable>();
		varLongWritables.add(new EntityPrefWritable(34L, 1.0f));
		varLongWritables.add(new EntityPrefWritable(56L, 2.0f));

		new ToUserVectorsReducer().reduce(new VarLongWritable(12L),
				varLongWritables, context);

		EasyMock.verify(context, userCounters);
	}

	/**
	 * tests {@link ToUserVectorsReducer} using boolean data
	 */
	@Test
	public void testToUserVectorReducerWithBooleanData() throws Exception {
		Reducer<VarLongWritable, VarLongWritable, VarLongWritable, VectorWritable>.Context context = EasyMock
				.createMock(Reducer.Context.class);
		Counter userCounters = EasyMock.createMock(Counter.class);

		EasyMock.expect(context.getCounter(ToUserVectorsReducer.Counters.USERS))
				.andReturn(userCounters);
		userCounters.increment(1);
		context.write(EasyMock.eq(new VarLongWritable(12L)), MathHelper
				.vectorMatches(
						MathHelper.elem(TasteHadoopUtils.idToIndex(34L), 1.0),
						MathHelper.elem(TasteHadoopUtils.idToIndex(56L), 1.0)));

		EasyMock.replay(context, userCounters);

		new ToUserVectorsReducer().reduce(new VarLongWritable(12L), Arrays
				.asList(new VarLongWritable(34L), new VarLongWritable(56L)),
				context);

		EasyMock.verify(context, userCounters);
	}

	/**
	 * tests {@link SimilarityMatrixRowWrapperMapper}
	 */
	@Test
	public void testSimilarityMatrixRowWrapperMapper() throws Exception {
		Mapper<IntWritable, VectorWritable, VarIntWritable, VectorOrPrefWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(
				EasyMock.eq(new VarIntWritable(12)),
				vectorOfVectorOrPrefWritableMatches(MathHelper.elem(34, 0.5),
						MathHelper.elem(56, 0.7)));

		EasyMock.replay(context);

		RandomAccessSparseVector vector = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		vector.set(12, 1.0);
		vector.set(34, 0.5);
		vector.set(56, 0.7);

		new SimilarityMatrixRowWrapperMapper().map(new IntWritable(12),
				new VectorWritable(vector), context);

		EasyMock.verify(context);
	}

	/**
	 * verifies the {@link Vector} included in a {@link VectorOrPrefWritable}
	 */
	private static VectorOrPrefWritable vectorOfVectorOrPrefWritableMatches(
			final Vector.Element... elements) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof VectorOrPrefWritable) {
					Vector v = ((VectorOrPrefWritable) argument).getVector();
					return MathHelper.consistsOf(v, elements);
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
	 * tests {@link UserVectorSplitterMapper}
	 */
	@Test
	public void testUserVectorSplitterMapper() throws Exception {
		Mapper<VarLongWritable, VectorWritable, VarIntWritable, VectorOrPrefWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(EasyMock.eq(new VarIntWritable(34)),
				prefOfVectorOrPrefWritableMatches(123L, 0.5f));
		context.write(EasyMock.eq(new VarIntWritable(56)),
				prefOfVectorOrPrefWritableMatches(123L, 0.7f));

		EasyMock.replay(context);

		UserVectorSplitterMapper mapper = new UserVectorSplitterMapper();
		setField(mapper, "maxPrefsPerUserConsidered", 10);

		RandomAccessSparseVector vector = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		vector.set(34, 0.5);
		vector.set(56, 0.7);

		mapper.map(new VarLongWritable(123L), new VectorWritable(vector),
				context);

		EasyMock.verify(context);
	}

	/**
	 * verifies a preference in a {@link VectorOrPrefWritable}
	 */
	private static VectorOrPrefWritable prefOfVectorOrPrefWritableMatches(
			final long userID, final float prefValue) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof VectorOrPrefWritable) {
					VectorOrPrefWritable pref = (VectorOrPrefWritable) argument;
					return pref.getUserID() == userID
							&& pref.getValue() == prefValue;
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
		Mapper<VarLongWritable, VectorWritable, VarIntWritable, VectorOrPrefWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(EasyMock.eq(new VarIntWritable(34)),
				prefOfVectorOrPrefWritableMatches(123L, 0.5f));
		context.write(EasyMock.eq(new VarIntWritable(56)),
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

		mapper.map(new VarLongWritable(123L), new VectorWritable(vector),
				context);
		mapper.map(new VarLongWritable(456L), new VectorWritable(vector),
				context);

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
		Mapper<VarLongWritable, VectorWritable, VarIntWritable, VectorOrPrefWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		context.write(EasyMock.eq(new VarIntWritable(34)),
				prefOfVectorOrPrefWritableMatchesNaN(123L));
		context.write(EasyMock.eq(new VarIntWritable(56)),
				prefOfVectorOrPrefWritableMatches(123L, 0.7f));

		EasyMock.replay(context);

		UserVectorSplitterMapper mapper = new UserVectorSplitterMapper();
		setField(mapper, "maxPrefsPerUserConsidered", 1);

		RandomAccessSparseVector vector = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		vector.set(34, 0.5);
		vector.set(56, 0.7);

		mapper.map(new VarLongWritable(123L), new VectorWritable(vector),
				context);

		EasyMock.verify(context);
	}

	/**
	 * verifies that a preference value is NaN in a {@link VectorOrPrefWritable}
	 */
	private static VectorOrPrefWritable prefOfVectorOrPrefWritableMatchesNaN(
			final long userID) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof VectorOrPrefWritable) {
					VectorOrPrefWritable pref = (VectorOrPrefWritable) argument;
					return pref.getUserID() == userID
							&& Float.isNaN(pref.getValue());
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
		Reducer<VarIntWritable, VectorOrPrefWritable, VarIntWritable, VectorAndPrefsWritable>.Context context = EasyMock
				.createMock(Reducer.Context.class);

		context.write(
				EasyMock.eq(new VarIntWritable(1)),
				vectorAndPrefsWritableMatches(Arrays.asList(123L, 456L),
						Arrays.asList(1.0f, 2.0f), MathHelper.elem(3, 0.5),
						MathHelper.elem(7, 0.8)));

		EasyMock.replay(context);

		Vector similarityColumn = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		similarityColumn.set(3, 0.5);
		similarityColumn.set(7, 0.8);

		VectorOrPrefWritable itemPref1 = new VectorOrPrefWritable(123L, 1.0f);
		VectorOrPrefWritable itemPref2 = new VectorOrPrefWritable(456L, 2.0f);
		VectorOrPrefWritable similarities = new VectorOrPrefWritable(
				similarityColumn);

		new ToVectorAndPrefReducer().reduce(new VarIntWritable(1),
				Arrays.asList(itemPref1, itemPref2, similarities), context);

		EasyMock.verify(context);
	}

	/**
	 * verifies a {@link VectorAndPrefsWritable}
	 */
	private static VectorAndPrefsWritable vectorAndPrefsWritableMatches(
			final List<Long> userIDs, final List<Float> prefValues,
			final Vector.Element... elements) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof VectorAndPrefsWritable) {
					VectorAndPrefsWritable vectorAndPrefs = (VectorAndPrefsWritable) argument;

					if (!vectorAndPrefs.getUserIDs().equals(userIDs)) {
						return false;
					}
					if (!vectorAndPrefs.getValues().equals(prefValues)) {
						return false;
					}
					return MathHelper.consistsOf(vectorAndPrefs.getVector(),
							elements);
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
	 * tests {@link ToVectorAndPrefReducer} in the error case that two
	 * similarity column vectors a supplied for the same item (which should
	 * never happen)
	 */
	@Test
	public void testToVectorAndPrefReducerExceptionOn2Vectors()
			throws Exception {
		Reducer<VarIntWritable, VectorOrPrefWritable, VarIntWritable, VectorAndPrefsWritable>.Context context = EasyMock
				.createMock(Reducer.Context.class);

		EasyMock.replay(context);

		Vector similarityColumn1 = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		Vector similarityColumn2 = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);

		VectorOrPrefWritable similarities1 = new VectorOrPrefWritable(
				similarityColumn1);
		VectorOrPrefWritable similarities2 = new VectorOrPrefWritable(
				similarityColumn2);

		try {
			new ToVectorAndPrefReducer().reduce(new VarIntWritable(1),
					Arrays.asList(similarities1, similarities2), context);
			fail();
		} catch (IllegalStateException e) {
			// good
		}

		EasyMock.verify(context);
	}

	/**
	 * tests
	 * {@link org.apache.mahout.cf.taste.hadoop.item.ItemFilterAsVectorAndPrefsReducer}
	 */
	static VectorAndPrefsWritable vectorAndPrefsForFilteringMatches(
			final long itemID, final long... userIDs) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof VectorAndPrefsWritable) {
					VectorAndPrefsWritable vectorAndPrefs = (VectorAndPrefsWritable) argument;
					Vector vector = vectorAndPrefs.getVector();
					if (vector.getNumNondefaultElements() != 1) {
						return false;
					}
					if (!Double.isNaN(vector.get(TasteHadoopUtils
							.idToIndex(itemID)))) {
						return false;
					}
					if (userIDs.length != vectorAndPrefs.getUserIDs().size()) {
						return false;
					}
					for (long userID : userIDs) {
						if (!vectorAndPrefs.getUserIDs().contains(userID)) {
							return false;
						}
					}
					return true;
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
	 * tests {@link PartialMultiplyMapper}
	 */
	@Test
	public void testPartialMultiplyMapper() throws Exception {

		Vector similarityColumn = new RandomAccessSparseVector(
				Integer.MAX_VALUE, 100);
		similarityColumn.set(3, 0.5);
		similarityColumn.set(7, 0.8);

		Mapper<VarIntWritable, VectorAndPrefsWritable, VarLongWritable, PrefAndSimilarityColumnWritable>.Context context = EasyMock
				.createMock(Mapper.Context.class);

		PrefAndSimilarityColumnWritable one = new PrefAndSimilarityColumnWritable();
		PrefAndSimilarityColumnWritable two = new PrefAndSimilarityColumnWritable();
		one.set(1.0f, similarityColumn);
		two.set(3.0f, similarityColumn);

		context.write(EasyMock.eq(new VarLongWritable(123L)), EasyMock.eq(one));
		context.write(EasyMock.eq(new VarLongWritable(456L)), EasyMock.eq(two));

		EasyMock.replay(context);

		VectorAndPrefsWritable vectorAndPrefs = new VectorAndPrefsWritable(
				similarityColumn, Arrays.asList(123L, 456L), Arrays.asList(
						1.0f, 3.0f));

		new PartialMultiplyMapper().map(new VarIntWritable(1), vectorAndPrefs,
				context);

		EasyMock.verify(context);
	}

	/**
	 * verifies a {@link RecommendedItemsWritable}
	 */
	static RecommsWritable recommendationsMatch(final RecommendedItem... items) {
		EasyMock.reportMatcher(new IArgumentMatcher() {
			@Override
			public boolean matches(Object argument) {
				if (argument instanceof RecommsWritable) {
					RecommsWritable recommendedItemsWritable = (RecommsWritable) argument;
					List<RecommendedItem> expectedItems = new LinkedList<RecommendedItem>(
							Arrays.asList(items));
					return expectedItems.equals(recommendedItemsWritable
							.getRecommendedItems());
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
	 * tests {@link RecommendedWritable}
	 */
	@Test
	public void testRecommendedItemsWritable() throws Exception {

		RecommsWritable recommended = new RecommsWritable();

		String[] items = { "25", "26", "27" };
		recommended.set(createRecommendations(1, items));

		assertEquals(recommended.toString(),
				"{\"25\":\"1.0\",\"26\":\"1.0\",\"27\":\"1.0\"}");
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
			}
			else if (userID == 2L) {
				assertEquals(1, items.size());
				assertEquals(2L, item1.getItemID());
				assertEquals(3.3, item1.getValue(), 0.05);
			}
			else if (userID == 3L) {
				assertEquals(2, items.size());
				assertEquals(3L, item1.getItemID());
				assertEquals(4.1, item1.getValue(), 0.05);
				RecommendedItem item2 = items.get(1);
				assertEquals(1L, item2.getItemID());
				assertEquals(3.7, item2.getValue(), 0.05);
			}
			else if (userID == 4L) {
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

		assertEquals(1L, item1.getItemID());
		assertEquals(3L, item2.getItemID());

		// predicted pref must be the sum of similarities:
		// item1: coocc(burger, hotdog) + coocc(burger, icecream) = 3
		// item2: coocc(berries, hotdog) + coocc(berries, icecream) = 2
		assertEquals(3, item1.getValue(), 0.05);
		assertEquals(2, item2.getValue(), 0.05);
	}

	static Map<Long, List<RecommendedItem>> readRecommendations(File file)
			throws IOException {
		Map<Long, List<RecommendedItem>> recommendations = Maps.newHashMap();
		Iterable<String> lineIterable = new FileLineIterable(file);
		for (String line : lineIterable) {
			String[] keyValue = line.split("\t");
			long userID = Long.parseLong(keyValue[0]);
			JSONObject json = (JSONObject) JSONSerializer.toJSON(keyValue[1]);
			JSONObject values = json.getJSONObject("value");
			Iterator<String> tokens = values.keys();

			List<RecommendedItem> items = new LinkedList<RecommendedItem>();
			while (tokens.hasNext()) {
				String itid = tokens.next().toString();
				long itemID = Long.parseLong(itid);
				float value = Float.parseFloat(values.getString(itid));
				items.add(new GenericRecommendedItem(itemID, value));
			}
			recommendations.put(userID, items);
		}
		return recommendations;
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
