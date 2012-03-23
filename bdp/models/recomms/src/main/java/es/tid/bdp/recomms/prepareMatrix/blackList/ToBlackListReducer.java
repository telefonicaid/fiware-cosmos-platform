package es.tid.bdp.recomms.prepareMatrix.blackList;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.apache.mahout.math.RandomAccessSparseVector;

import org.apache.hadoop.io.LongWritable;
import org.apache.mahout.math.Vector;
import org.bson.BSONObject;

import com.mongodb.BasicDBObject;

public final class ToBlackListReducer extends
        Reducer<LongWritable, LongWritable, Text, BSONObject> {
    public BasicDBObject toObject(Vector id) {
        BasicDBObject json = new BasicDBObject();
        json.put("value", id.toString());
        return json;
    }

    @Override
    public void reduce(LongWritable index,
            Iterable<LongWritable> possibleItemIDs, Context context)
            throws IOException, InterruptedException {
        Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 100);
        for (LongWritable item : possibleItemIDs) {
            int item_index = TasteHadoopUtils.idToIndex(item.get());
            userVector.set(item_index, 1);
        }

        Text LuserId = new Text();
        LuserId.set(index.toString());

        context.write(LuserId, toObject(userVector));
    }

}