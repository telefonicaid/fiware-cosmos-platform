package es.tid.ps.profile.userprofile;

import java.io.IOException;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.profile.categoryextraction.CategoryInformation;
import es.tid.ps.profile.categoryextraction.CompositeKey;

/**
 * Maps <[user, url], categoryInfo> to <userId, [category, count]>
 *
 * @author sortega@tid.es
 */
public class UserProfileMapper extends Mapper<CompositeKey, CategoryInformation,
        Text, CategoryCount> {
    private Text userId;
    private CategoryCount categoryCount;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.userId = new Text();
        this.categoryCount = new CategoryCount();
    }

    @Override
    protected void map(CompositeKey key, CategoryInformation categoryInfo,
            Context context)
            throws IOException, InterruptedException {
        this.userId.set(categoryInfo.getUserId());
        for (String category : categoryInfo.getCategoryNames()) {
            this.categoryCount.setCategory(category);
            this.categoryCount.setCount(categoryInfo.getCount());
            context.write(this.userId, this.categoryCount);
        }
    }
}
