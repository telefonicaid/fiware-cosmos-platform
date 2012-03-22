package es.tid.bdp.recomms.extractRecomms;

import com.google.common.primitives.Floats;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.RecommenderTIDJob;
import es.tid.bdp.recomms.common.TIDUtils;
import es.tid.bdp.recomms.data.RecommsProtocol.Predictions;
import es.tid.bdp.recomms.data.RecommsProtocol.Predictions.Pred;
import es.tid.bdp.recomms.data.RecommsProtocol.PrefandSimil;
import es.tid.bdp.recomms.data.RecommsProtocol.VectorElement;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.mahout.common.iterator.FileLineIterable;
import org.apache.mahout.common.HadoopUtil;
import org.apache.hadoop.io.LongWritable;

import org.apache.mahout.math.RandomAccessSparseVector;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.function.DoubleFunction;
import org.apache.mahout.math.map.OpenIntLongHashMap;
import org.apache.mahout.cf.taste.common.TopK;
import org.apache.mahout.cf.taste.impl.common.FastIDSet;

import java.io.IOException;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * 
 * @author jaume
 * 
 *         This class calculates predictions for each user
 */
public final class AggrAndRecomObjReducer
        extends
        Reducer<LongWritable, ProtobufWritable<PrefandSimil>, Text, ProtobufWritable<Predictions>> {
    private static final Logger log = LoggerFactory
            .getLogger(AggrAndRecomObjReducer.class);

    private ProtobufWritable<Predictions> recomms;

    public static final String ITEMID_INDEX_PATH = "itemIDIndexPath";
    public static final String NUM_RECOMMENDATIONS = "numRecommendations";
    static final int DEFAULT_NUM_RECOMMENDATIONS = 20;
    static final String ITEMS_FILE = "itemsFile";

    private boolean booleanData;
    private int recommendationsPerUser;
    private FastIDSet itemsToRecommendFor;
    private OpenIntLongHashMap indexItemIDMap;

    private static final float BOOLEAN_PREF_VALUE = 1.0f;
    private static final Comparator<Pred> BY_PREFERENCE_VALUE = new Comparator<Pred>() {
        @Override
        public int compare(Pred one, Pred two) {
            return Floats.compare(one.getValue(), two.getValue());
        }
    };

    @Override
    protected void setup(Context context) throws IOException {

        this.recomms = ProtobufWritable.newInstance(Predictions.class);
        Configuration conf = context.getConfiguration();
        recommendationsPerUser = conf.getInt(NUM_RECOMMENDATIONS,
                DEFAULT_NUM_RECOMMENDATIONS);
        booleanData = conf.getBoolean(RecommenderTIDJob.BOOLEAN_DATA, false);
        indexItemIDMap = TIDUtils.readItemIDIndexMap(
                conf.get(ITEMID_INDEX_PATH), conf);

        String itemFilePathString = conf.get(ITEMS_FILE);
        if (itemFilePathString != null) {
            itemsToRecommendFor = new FastIDSet();
            for (String line : new FileLineIterable(HadoopUtil.openStream(
                    new Path(itemFilePathString), conf))) {
                try {
                    itemsToRecommendFor.add(Long.parseLong(line));
                } catch (NumberFormatException nfe) {
                    log.warn("itemsFile line ignored: {}", line);
                }
            }
        }
    }

    private static final DoubleFunction ABSOLUTE_VALUES = new DoubleFunction() {
        @Override
        public double apply(double value) {
            return value < 0 ? value * -1 : value;
        }
    };

    @Override
    public void reduce(LongWritable userID,
            Iterable<ProtobufWritable<PrefandSimil>> values, Context context)
            throws IOException, InterruptedException {
        if (booleanData) {
            reduceBooleanData(userID, values, context);
        } else {
            reduceNonBooleanData(userID, values, context);
        }
    }

    private void reduceBooleanData(LongWritable userID,
            Iterable<ProtobufWritable<PrefandSimil>> values, Context context)
            throws IOException, InterruptedException {
        /*
         * having boolean data, each estimated preference can only be 1, however
         * we can't use this to rank the recommended items, so we use the sum of
         * similarities for that.
         */
        Vector predictionVector = null;
        for (ProtobufWritable<PrefandSimil> prefAndSimilarityColumn : values) {
            prefAndSimilarityColumn.setConverter(PrefandSimil.class);
            List<VectorElement> vList = prefAndSimilarityColumn.get()
                    .getVectorList();
            if (predictionVector == null) {
                for (VectorElement vElem : vList) {
                    predictionVector = new RandomAccessSparseVector(
                            Integer.MAX_VALUE, prefAndSimilarityColumn.get()
                                    .getVectorCount());
                    predictionVector.setQuick((int) vElem.getIndex(),
                            vElem.getValue());
                }
            } else {
                for (VectorElement vElem : vList) {
                    double newValue = predictionVector.get((int) vElem
                            .getIndex());
                    predictionVector.set((int) vElem.getIndex(),
                            vElem.getValue() + newValue);
                }
            }
        }
        writeRecommendedItems(userID, predictionVector, context);
    }

    private void reduceNonBooleanData(LongWritable userID,
            Iterable<ProtobufWritable<PrefandSimil>> values, Context context)
            throws IOException, InterruptedException {
        /* each entry here is the sum in the numerator of the prediction formula */
        Vector numerators = null;
        /*
         * each entry here is the sum in the denominator of the prediction
         * formula
         */
        Vector denominators = null;
        /*
         * each entry here is the number of similar items used in the prediction
         * formula
         */
        Vector numberOfSimilarItemsUsed = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);

        for (ProtobufWritable<PrefandSimil> prefAndSimilarityColumn : values) {
            prefAndSimilarityColumn.setConverter(PrefandSimil.class);
            Vector simColumn = new RandomAccessSparseVector(Integer.MAX_VALUE,
                    prefAndSimilarityColumn.get().getVectorCount());

            float prefValue = prefAndSimilarityColumn.get().getPrefvalue();
            List<VectorElement> vList = prefAndSimilarityColumn.get()
                    .getVectorList();

            for (VectorElement vElem : vList) {
                simColumn.set((int) vElem.getIndex(), vElem.getValue());
            }
            /* count the number of items used for each prediction */
            Iterator<Vector.Element> usedItemsIterator = simColumn
                    .iterateNonZero();
            while (usedItemsIterator.hasNext()) {
                int itemIDIndex = usedItemsIterator.next().index();
                numberOfSimilarItemsUsed.setQuick(itemIDIndex,
                        numberOfSimilarItemsUsed.getQuick(itemIDIndex) + 1);
            }

            numerators = numerators == null ? prefValue == BOOLEAN_PREF_VALUE ? simColumn
                    .clone() : simColumn.times(prefValue)
                    : numerators
                            .plus(prefValue == BOOLEAN_PREF_VALUE ? simColumn
                                    : simColumn.times(prefValue));

            simColumn.assign(ABSOLUTE_VALUES);
            denominators = denominators == null ? simColumn : denominators
                    .plus(simColumn);
        }

        if (numerators == null) {
            return;
        }

        Vector recommendationVector = new RandomAccessSparseVector(
                Integer.MAX_VALUE, 100);
        Iterator<Vector.Element> iterator = numerators.iterateNonZero();
        while (iterator.hasNext()) {
            Vector.Element element = iterator.next();
            int itemIDIndex = element.index();
            if (numberOfSimilarItemsUsed.getQuick(itemIDIndex) > 1) {
                double prediction = element.get()
                        / denominators.getQuick(itemIDIndex);
                recommendationVector.setQuick(itemIDIndex, prediction);
            }
        }

        writeRecommendedItems(userID, recommendationVector, context);
    }

    /**
     * find the top entries in recommendationVector, map them to the real
     * itemIDs and write back the result
     */
    private void writeRecommendedItems(LongWritable userID,
            Vector recommendationVector, Context context) throws IOException,
            InterruptedException {

        TopK<Pred> topKItems = new TopK<Pred>(recommendationsPerUser,
                BY_PREFERENCE_VALUE);

        Iterator<Vector.Element> recommendationVectorIterator = recommendationVector
                .iterateNonZero();

        while (recommendationVectorIterator.hasNext()) {
            Vector.Element element = recommendationVectorIterator.next();

            int index = element.index();
            long itemID;

            if (indexItemIDMap != null && !indexItemIDMap.isEmpty()) {
                itemID = indexItemIDMap.get(index);
            } else {
                itemID = index;
            }
            if (itemsToRecommendFor == null
                    || itemsToRecommendFor.contains(itemID)) {
                float prefValue = (float) element.get();
                if (!Float.isNaN(prefValue)) {
                    topKItems.offer(Pred.newBuilder().setItemID(itemID)
                            .setValue(prefValue).build());
                }
            }
        }
        this.recomms.set(Predictions.newBuilder()
                .addAllPredicts(topKItems.retrieve()).build());
        if (this.recomms.get().getPredictsCount() > 0) {

            context.write(new Text(userID.toString()), this.recomms);
        }
    }

}
