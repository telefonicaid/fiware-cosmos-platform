package es.tid.ps.profile.userprofile;

import java.io.IOException;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.categoryextraction.CategoryInformation;

/**
 * Maps <[userId, date], categoryInfo> to <[userId, date], [category, count]>
 *
 * @author sortega@tid.es
 */
public class UserProfileMapper extends Mapper<BinaryKey, CategoryInformation,
                                              BinaryKey, CategoryCount> {
    private CategoryCount categoryCount;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.categoryCount = new CategoryCount();
    }

    @Override
    protected void map(BinaryKey userDateKey, CategoryInformation categoryInfo,
            Context context) throws IOException, InterruptedException {
        for (String category : categoryInfo.getCategoryNames()) {
            this.categoryCount.setCategory(category);
            this.categoryCount.setCount(categoryInfo.getCount());
            context.write(userDateKey, this.categoryCount);
        }
    }
}
