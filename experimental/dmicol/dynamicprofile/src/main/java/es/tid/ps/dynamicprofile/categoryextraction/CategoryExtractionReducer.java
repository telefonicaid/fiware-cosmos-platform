package es.tid.ps.dynamicprofile.categoryextraction;

import java.io.IOException;

import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.dynamicprofile.dictionary.DictionaryHandler;
import es.tid.ps.kpicalculation.data.WebLog;

public class CategoryExtractionReducer extends
        Reducer<CompositeKey, WebLog, CompositeKey, CategoryInformation> {
    private final static String CATEGORY_DELIMITER = "/";

    @Override
    public void setup(Context context) throws IOException {
        DictionaryHandler.init("", "", "", "");
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<WebLog> values,
            Context context) throws IOException, InterruptedException {
        // Use the comScore API
        String url = values.iterator().next().fullUrl;
        String categories = DictionaryHandler.getUrlCategories(url);

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
