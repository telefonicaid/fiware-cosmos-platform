package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.profile.dictionary.DictionaryHandler;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
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
            context.getCounter(CategoryExtractionCounter.EMPTY_CATEGORY).
                    increment(1L);
            return;
        }
        context.getCounter(CategoryExtractionCounter.VALID_CATEGORY).
                increment(1L);

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
