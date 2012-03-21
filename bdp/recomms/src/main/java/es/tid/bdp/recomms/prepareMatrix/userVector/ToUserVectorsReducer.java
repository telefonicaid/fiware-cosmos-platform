package es.tid.bdp.recomms.prepareMatrix.userVector;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Reducer;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.apache.mahout.math.RandomAccessSparseVector;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;

import org.apache.hadoop.io.LongWritable;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.EntityPref;
import es.tid.bdp.recomms.importer.ItemIDIndexObjMapper;
import es.tid.bdp.recomms.importer.ItemIDIndexObjReducer;
import es.tid.bdp.recomms.prepareMatrix.userVector.ToUserVectorsCounter;
import es.tid.bdp.recomms.prepareMatrix.userVector.ToUserVectorsReducer;

/**
 * <h1>Input</h1>
 * 
 * <p>
 * Takes user IDs as {@link LongWritable} mapped to all associated item IDs and
 * preference values, as {@link ProtobufWritable>entityPref>}s.
 * </p>
 * 
 * <h1>Output</h1>
 * 
 * <p>
 * The same user ID mapped to a {@link RandomAccessSparseVector} representation
 * of the same item IDs and preference values. Item IDs are used as vector
 * indexes; they are hashed into ints to work as indexes with
 * {@link TasteHadoopUtils#idToIndex(long)}. The mapping is remembered for later
 * with a combination of {@link ItemIDIndexObjMapper} and
 * {@link ItemIDIndexObjReducer}.
 * </p>
 */
public final class ToUserVectorsReducer
        extends
        Reducer<LongWritable, ProtobufWritable<EntityPref>, LongWritable, VectorWritable> {
    public static final String MIN_PREFERENCES_PER_USER = ToUserVectorsReducer.class
            .getName() + ".minPreferencesPerUser";

    private int minPreferences;

    @Override
    protected void setup(Context ctx) throws IOException, InterruptedException {
        super.setup(ctx);
        minPreferences = ctx.getConfiguration().getInt(
                MIN_PREFERENCES_PER_USER, 1);
    }

    @Override
    public void reduce(LongWritable userID,
            Iterable<ProtobufWritable<EntityPref>> itemPrefs, Context context)
            throws IOException, InterruptedException {
        Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 1);
        for (ProtobufWritable<EntityPref> itemPref : itemPrefs) {
            itemPref.setConverter(EntityPref.class);
            int index = TasteHadoopUtils.idToIndex(itemPref.get().getItemID());
            float value = itemPref.get().getValue();
            userVector.set(index, value);
        }
        if (userVector.getNumNondefaultElements() >= minPreferences) {
            context.getCounter(ToUserVectorsCounter.USERS).increment(1);
            context.write(userID, new VectorWritable(userVector));
        }
    }

}
