package es.tid.bdp.recomms.prepareMatrix.blackList;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.LongWritable;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;

public final class ToBlackListMapper
        extends
        Mapper<LongWritable, ProtobufWritable<UserPref>, LongWritable, LongWritable> {
    @Override
    public void map(LongWritable key, ProtobufWritable<UserPref> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(UserPref.class);
        long itemID = Long.valueOf(value.get().getItemId());

        if (value.get().getEventType() == 0 || value.get().getEventType() == 1) {
            context.write(key, new LongWritable(itemID));
        }

    }
}
