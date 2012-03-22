package es.tid.bdp.recomms.importer;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Mapper;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.mahout.cf.taste.hadoop.TasteHadoopUtils;

import org.bson.BSONObject;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;

public final class ItemIDIndexObjMapper extends
        Mapper<Object, Object, IntWritable, LongWritable> {
    public boolean transpose = false;

    @Override
    protected void setup(Context context) {
    }

    @Override
    public void map(Object key, Object value, Context context)
            throws IOException, InterruptedException {

        long itemID;
        if (value instanceof BSONObject) {
            BSONObject bsonValue = (BSONObject) value;
            itemID = Long.valueOf(bsonValue.get("item_id").toString());
        } else {
            @SuppressWarnings("unchecked")
            ProtobufWritable<UserPref> protoValue = (ProtobufWritable<UserPref>) value;
            protoValue.setConverter(UserPref.class);
            itemID = Long.valueOf(protoValue.get().getItemId());
        }
        int index = TasteHadoopUtils.idToIndex(itemID);
        context.write(new IntWritable(index), new LongWritable(itemID));
    }
}
