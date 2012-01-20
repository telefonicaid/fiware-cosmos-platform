package es.tid.ps.dynamicprofile.categoryextraction;

import java.io.IOException;
import java.util.Iterator;

import es.tid.ps.dynamicprofile.dictionary.DictionaryHandler;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

public class CategoryExtractionReducer extends
        Reducer<CompositeKey, NullWritable, CompositeKey, CategoryInformation> {
    private final static String CATEGORY_DELIMITER = "/";

    @Override
    public void setup(Context context) throws IOException {
        DictionaryHandler.init(DistributedCache.getLocalCacheFiles(context
                .getConfiguration()));
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<NullWritable> values,
            Context context) throws IOException, InterruptedException {
        // Use the comScore API
        String url = key.getSecondaryKey();
        String categories = getCategories(url);
        if (categories == null || categories.isEmpty()) {
            context.getCounter(CategoryExtractionCounter.EMPTY_CATEGORY).increment(1L);
            return;
        }

        context.getCounter(CategoryExtractionCounter.VALID_CATEGORY).increment(1L);

        CategoryInformation cat = new CategoryInformation(key.getPrimaryKey(),
                key.getSecondaryKey(), countURLInstances(values),
                categories.split(CATEGORY_DELIMITER));
        context.write(key, cat);
    }

    protected String getCategories(String url) {
        return DictionaryHandler.getUrlCategories(url);
    }

    private long countURLInstances(Iterable<NullWritable> values) {
        long count = 0;
        Iterator<NullWritable> it = values.iterator();
        while (it.hasNext()) {
            count++;
            it.next();
        }
        return count;
    }
}
