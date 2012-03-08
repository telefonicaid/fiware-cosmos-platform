package org.apache.mahout.tid.impl.recommender.hadoop.preparation;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import org.apache.mahout.math.RandomAccessSparseVector;

import org.apache.mahout.math.VarLongWritable;
import org.apache.mahout.math.Vector;
import org.apache.mahout.math.VectorWritable;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;
import org.bson.BSONObject;

import com.mongodb.BasicDBObject;

public final class ToBlackListReducer extends
        Reducer<VarLongWritable, VarLongWritable, Text, BSONObject> {
    public BasicDBObject toObject(VectorWritable id) {
        BasicDBObject json = new BasicDBObject();
        json.put("value", id.toString());
        return json;
    }

    @Override
    public void reduce(VarLongWritable index,
            Iterable<VarLongWritable> possibleItemIDs, Context context)
            throws IOException, InterruptedException {
        Vector userVector = new RandomAccessSparseVector(Integer.MAX_VALUE, 100);
        for (VarLongWritable item : possibleItemIDs) {
            int item_index = TasteHadoopUtils.idToIndex(item.get());
            userVector.set(item_index, 1);
        }

        VectorWritable vw = new VectorWritable(userVector);
        vw.setWritesLaxPrecision(true);

        Text LuserId = new Text();
        LuserId.set(index.toString());

        context.write(LuserId, toObject(vw));
    }

}