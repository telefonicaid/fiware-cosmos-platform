package es.tid.cosmos.profile.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.io.BSONWritable;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<Text,
        ProtobufWritable<UserProfile>, MongoProperty, BSONWritable> {
    private MongoProperty id;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.id = new MongoProperty();
        this.id.setProperty("id");
    }

    @Override
    public void reduce(Text userId,
            Iterable<ProtobufWritable<UserProfile>> profiles, Context context)
            throws IOException, InterruptedException {
        this.id.setValue(userId.toString());
        for (ProtobufWritable<UserProfile> wrappedProfile : profiles) {
            wrappedProfile.setConverter(UserProfile.class);
            context.write(this.id, toBSON(wrappedProfile.get()));
        }
    }

    private BSONWritable toBSON(UserProfile profile) {
        BSONObject obj = new BasicBSONObject();
        obj.put("date", profile.getDate());

        BSONObject categories = new BasicBSONObject();
        for (final CategoryCount pair: profile.getCountsList()) {
            categories.put(pair.getName(), pair.getCount());
        }
        obj.put("categories", categories);

        return new BSONWritable(obj);
    }
}
