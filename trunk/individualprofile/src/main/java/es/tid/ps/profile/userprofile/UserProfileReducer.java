package es.tid.ps.profile.userprofile;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

/**
 * Aggregates category counts into a user profile.
 * <user, [category count]> -> <user, profile>
 *
 * @author sortega@tid.es
 */
public class UserProfileReducer extends Reducer<Text, CategoryCount, Text,
        UserProfile> {
    private UserProfile profile;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.profile = new UserProfile();
    }

    @Override
    protected void reduce(Text userId, Iterable<CategoryCount> counts,
            Context context) throws IOException, InterruptedException {
        this.profile.setUserId(userId.toString());
        for (CategoryCount count : counts) {
            this.profile.add(count);
        }
        context.write(userId, this.profile);
    }
}
