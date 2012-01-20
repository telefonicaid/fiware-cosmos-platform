package es.tid.ps.dynamicprofile.userprofile;

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
    @Override
    protected void reduce(Text userId, Iterable<CategoryCount> counts,
            Context context) throws IOException, InterruptedException {
        UserProfile profile = new UserProfile();
        profile.setUserId(userId.toString());
        for (CategoryCount count : counts) {
            profile.add(count);
        }
        context.write(userId, profile);
    }
}
