package es.tid.bdp.recomms.prepareMatrix.userVector;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.LongWritable;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.RecommenderTIDJob;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;
import es.tid.bdp.recomms.data.RecommsProtocol.EntityPref;

import java.io.IOException;

public class ToElementPrefsMapper
        extends
        Mapper<LongWritable, ProtobufWritable<UserPref>, LongWritable, ProtobufWritable<EntityPref>> {
    private boolean booleanData;

    public ToElementPrefsMapper() {
    }

    @Override
    protected void setup(Context context) {
        Configuration jobConf = context.getConfiguration();
        booleanData = jobConf.getBoolean(RecommenderTIDJob.BOOLEAN_DATA, false);
    }

    @Override
    public void map(LongWritable key, ProtobufWritable<UserPref> value,
            Context context) throws IOException, InterruptedException {

        long userID = Long.parseLong(key.toString());
        value.setConverter(UserPref.class);
        long itemID = value.get().getItemId();
        float prefValue = booleanData ? 1.0f : value.get().getEventValue();

        ProtobufWritable<EntityPref> eprf = new ProtobufWritable<EntityPref>();
        eprf.setConverter(EntityPref.class);
        eprf.set(EntityPref.newBuilder().setItemID(itemID).setValue(prefValue)
                .build());

        context.write(new LongWritable(userID), eprf);
    }
}
