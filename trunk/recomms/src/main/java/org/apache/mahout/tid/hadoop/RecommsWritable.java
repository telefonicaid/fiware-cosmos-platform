package org.apache.mahout.tid.hadoop;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import com.google.common.collect.Lists;
import com.mongodb.BasicDBObject;

import org.apache.hadoop.io.Writable;
import org.apache.mahout.cf.taste.impl.recommender.GenericRecommendedItem;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.math.Varint;

/**
 * A {@link Writable} which encapsulates a list of {@link RecommendedItem}s.
 * This is the mapper (and reducer) output, and represents items recommended to
 * a user. The first item is the one whose estimated preference is highest.
 */
public final class RecommsWritable implements Writable {
    private List<RecommendedItem> recommended;

    public RecommsWritable() {
    }

    public RecommsWritable(List<RecommendedItem> recommended) {
        this.recommended = recommended;
    }

    public List<RecommendedItem> getRecommendedItems() {
        return recommended;
    }

    public void set(List<RecommendedItem> recommended) {
        this.recommended = recommended;
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeInt(recommended.size());
        System.out.println(out);
        for (RecommendedItem item : recommended) {
            Varint.writeSignedVarLong(item.getItemID(), out);
            out.writeFloat(item.getValue());
        }

    }

    @Override
    public void readFields(DataInput in) throws IOException {
        int size = in.readInt();
        recommended = Lists.newArrayListWithCapacity(size);
        for (int i = 0; i < size; i++) {
            long itemID = Varint.readSignedVarLong(in);
            float value = in.readFloat();
            RecommendedItem recommendedItem = new GenericRecommendedItem(
                    itemID, value);
            recommended.add(recommendedItem);
        }
    }

    @Override
    public String toString() {
        StringBuilder result = new StringBuilder();

        result.append('{');
        Iterator<RecommendedItem> recomms = recommended.listIterator();
        while (recomms.hasNext()) {
        	RecommendedItem item = recomms.next();
            result.append('"');
            result.append(String.valueOf(item.getItemID()));
            result.append('"');
            result.append(':');
            result.append('"');
            result.append(String.valueOf(item.getValue()));
            result.append('"');
            
            if(recomms.hasNext()) {
                result.append(',');
            }
        }
        result.append('}');
        return result.toString();
    }

    public BasicDBObject toObject() {

        BasicDBObject result = new BasicDBObject();

        for (RecommendedItem item : recommended) {
            result.put(String.valueOf(item.getItemID()),
                    String.valueOf(item.getValue()));
        }

        BasicDBObject json = new BasicDBObject();
        json.put("value", result);

        return json;
    }
}
