package es.tid.bdp.recomms.importer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.LongWritable;
import org.bson.BSONObject;

import es.tid.bdp.recomms.RecommenderTIDJob;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.bdp.recomms.data.RecommsProtocol.UserPref;

import java.io.IOException;

public class ToEventPrefsMapper extends
        Mapper<Object, Object, LongWritable, ProtobufWritable<UserPref>> {
    private boolean booleanData;

    private ProtobufWritable<UserPref> userPrefs;

    @Override
    protected void setup(Context context) {
        Configuration jobConf = context.getConfiguration();
        booleanData = jobConf.getBoolean(RecommenderTIDJob.BOOLEAN_DATA, false);
        
    }

    private enum event_types {
        PURCHASE, RATE, RECODISPLAY, CLICK, VIEW
    };

    public ToEventPrefsMapper() {
        this.userPrefs = ProtobufWritable.newInstance(UserPref.class);
    }

    @Override
    public void map(Object key, Object value, Context context)
            throws IOException, InterruptedException {

        long userID;
        long itemID;
        float eventValue;
        int eventType;
        String type = new String();
        String date = new String();
        String deviceId = new String();
        if (value instanceof BSONObject) {
            BSONObject BSONvalue = (BSONObject) value;
            userID = Long.valueOf(BSONvalue.get("user_id").toString());
            itemID = Long.valueOf(BSONvalue.get("item_id").toString());
            deviceId = BSONvalue.get("device_id").toString();
            eventValue = Float.parseFloat(BSONvalue.get("eventValue")
                    .toString());
            type = BSONvalue.get("event_type").toString();
            date = BSONvalue.get("created_at").toString();
        } else {
            Text valueText = (Text) value;
            String[] st = valueText.toString().split(",");
            userID = Long.valueOf(st[0]);
            itemID = Long.valueOf(st[3]);
            if (booleanData) {
                eventValue = 1.0f;
            } else {
                eventValue = Float.parseFloat(st[4]);
            }
            type = st[2];
            deviceId = st[1];
            //date = st[5]; //TODO CONSIDER DATE FROM FILE
        }

        event_types etype = Enum.valueOf(ToEventPrefsMapper.event_types.class,
                type.toUpperCase().toString());

        switch (etype) {
        case PURCHASE:
            eventType = 0;
            break;
        case RATE:
            eventType = 1;
            break;
        case RECODISPLAY:
            eventType = 2;
            break;
        case CLICK:
            eventType = 3;
            break;
        case VIEW:
            eventType = 4;
            break;
        default:
            eventType = 1;
            break;
        }
        
        UserPref.Builder uPref = UserPref.newBuilder();

        uPref.setUserID(userID)
        .setEventType(eventType)
        .setItemId(itemID)
        .setEventValue(eventValue).setDeviceID(deviceId).setEventDate(date);
        
        this.userPrefs.set(uPref.build());
        context.write(new LongWritable(userID), this.userPrefs);
    }

}
