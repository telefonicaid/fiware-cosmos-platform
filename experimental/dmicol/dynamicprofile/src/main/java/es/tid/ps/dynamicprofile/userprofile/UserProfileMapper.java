package es.tid.ps.dynamicprofile.userprofile;

import es.tid.ps.dynamicprofile.categoryextraction.CategoryInformation;
import es.tid.ps.dynamicprofile.categoryextraction.CompositeKey;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import java.io.IOException;

/**
 * Maps <[user, url], categoryInfo> to <userId, [category, count]>
 * @author sortega@tid.es
 */
public class UserProfileMapper extends Mapper<CompositeKey, CategoryInformation, Text, CategoryCount> {

    @Override
    protected void map(CompositeKey key, CategoryInformation categoryInfo, Context context)
            throws IOException, InterruptedException {

        Text userId = new Text(categoryInfo.getUserId());
        for (String category: categoryInfo.getCategoryNames()) {
            context.write(userId, new CategoryCount(category, categoryInfo.getCount()));
        }
    }
}
