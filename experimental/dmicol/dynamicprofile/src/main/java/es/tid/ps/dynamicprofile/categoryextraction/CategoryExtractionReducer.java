package es.tid.ps.dynamicprofile.categoryextraction;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.dynamicprofile.dictionary.DictionaryHandler;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.io.NullWritable;

public class CategoryExtractionReducer extends
        Reducer<CompositeKey, NullWritable, CompositeKey, CategoryInformation> {
    private final static String CATEGORY_DELIMITER = "/";

    @Override
    public void setup(Context context) throws IOException {
        DictionaryHandler.init(DistributedCache.getLocalCacheFiles(
                context.getConfiguration()));
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<NullWritable> values,
            Context context) throws IOException, InterruptedException {
        // Use the comScore API
        String url = key.getSecondaryKey();
        String categories = DictionaryHandler.getUrlCategories(url);
        if (categories == null || categories.isEmpty()) {
            context.getCounter(CategoryExtractionCounter.EMPTY_CATEGORY).increment(1L);
            return;
        }

        context.getCounter(CategoryExtractionCounter.VALID_CATEGORY).increment(1L);

        // Count the number of instances of the same URL
        long count = 0;
        while (values.iterator().hasNext()) {
            count++;
        }

        CategoryInformation cat = new CategoryInformation(key.getPrimaryKey(),
                key.getSecondaryKey(), count,
                categories.split(CATEGORY_DELIMITER));
        context.write(key, cat);
    }
}
