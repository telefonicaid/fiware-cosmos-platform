package es.tid.ps.profile.userprofile;

import es.tid.ps.profile.categoryextraction.CompositeKey;
import java.io.IOException;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Aggregates category counts into a user profile.
 * <[userId, date], [category, count]> -> <userId, profile>
 *
 * @author sortega@tid.es
 */
public class UserProfileReducer extends Reducer<CompositeKey, CategoryCount, 
                                                Text, UserProfile> {
    private UserProfile profile;
    private Text userId;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.profile = new UserProfile();
        this.userId = new Text();
    }

    @Override
    protected void reduce(CompositeKey userDateKey, Iterable<CategoryCount> counts,
            Context context) throws IOException, InterruptedException {
        this.profile.reset();
        this.profile.setUserId(userDateKey.getPrimaryKey());
        this.profile.setDate(userDateKey.getSecondaryKey());
        for (CategoryCount count : counts) {
            this.profile.add(count);
        }
        this.userId.set(userDateKey.getPrimaryKey());
        context.write(this.userId, this.profile);
    }
}
