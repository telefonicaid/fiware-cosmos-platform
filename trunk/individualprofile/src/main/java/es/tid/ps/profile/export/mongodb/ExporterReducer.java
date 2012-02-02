package es.tid.ps.profile.export.mongodb;

import com.mongodb.hadoop.io.BSONWritable;
import es.tid.ps.profile.userprofile.UserProfile;
import java.io.IOException;
import java.util.Date;
import java.util.Iterator;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author dmicol, sortega
 */
public class ExporterReducer extends Reducer<Text, UserProfile,
                                             MongoProperty, BSONWritable> {
    private MongoProperty id;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.id = new MongoProperty();
        this.id.setProperty("id");
    }
    
    @Override
    public void reduce(Text userId, Iterable<UserProfile> profiles,
                       Context context) throws IOException, 
                                               InterruptedException {
        for (Iterator<UserProfile> it = profiles.iterator(); it.hasNext();) {
            id.setValue(userId.toString());
            context.write(id, toBSON(it.next()));
        }
    }

    private BSONWritable toBSON(UserProfile profile) {
        BSONObject obj = new BasicBSONObject();
        obj.put("date", profile.getDate());
        obj.put("categories", profile.getCategoryCounts());
        return new BSONWritable(obj);
    }
}
