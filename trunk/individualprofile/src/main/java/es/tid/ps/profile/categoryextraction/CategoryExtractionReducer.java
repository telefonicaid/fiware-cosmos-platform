package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.Dictionary;
import es.tid.ps.profile.dictionary.comscore.CSDictionary;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends Reducer<CompositeKey,
        UserNavigation, CompositeKey, CategoryInformation> {
    private static Dictionary dictionary = null;
    private CategoryInformation catInfo;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);
        this.catInfo = new CategoryInformation();
    }

    protected void setupDictionary(Context context) throws IOException {
        if (dictionary == null) {
            dictionary = new CSDictionary(
                    DistributedCache.getLocalCacheFiles(
                    context.getConfiguration()));
            dictionary.init();
        }
    }

    @Override
    protected void reduce(CompositeKey key, Iterable<UserNavigation> values,
            Context context) throws IOException, InterruptedException {
        Map<String, Long> uniqueUrlCounts = this.getUniqueUrlCounts(values);
        for (String url : uniqueUrlCounts.keySet()) {
            long urlInstances = uniqueUrlCounts.get(url);
            Categorization dictionaryResponse = this.categorize(url);

            switch (dictionaryResponse.result) {
                case KNOWN_URL:
                    context.getCounter(CategoryExtractionCounter.KNOWN_VISITS).
                            increment(urlInstances);
                    this.catInfo.setUserId(key.getPrimaryKey());
                    this.catInfo.setUrl(url);
                    this.catInfo.setDate(key.getSecondaryKey());
                    this.catInfo.setCount(urlInstances);
                    this.catInfo.setCategoryNames(
                            dictionaryResponse.categories);
                    context.write(key, this.catInfo);
                    break;

                case IRRELEVANT_URL:
                    context.getCounter(
                            CategoryExtractionCounter.IRRELEVANT_VISITS).
                            increment(urlInstances);
                    break;

                case UNKNOWN_URL:
                    context.getCounter(
                            CategoryExtractionCounter.UNKNOWN_VISITS).
                            increment(urlInstances);
                    // TODO: do something smart for URL category extraction.
                    break;

                case GENERIC_FAILURE:
                    context.getCounter(
                            CategoryExtractionCounter.UNPROCESSED_VISITS).
                            increment(urlInstances);
                    break;

                default:
                    throw new IllegalStateException(
                            "Invalid dictionary response.");
            }
        }
    }

    protected Categorization categorize(String url) {
        return dictionary.categorize(url);
    }
    
    private Map<String, Long> getUniqueUrlCounts(
            Iterable<UserNavigation> values) {
        Map<String, Long> uniqueUrlCounts = new HashMap<String, Long>();
        for (UserNavigation nav : values) {
            Long count;
            if (uniqueUrlCounts.containsKey(nav.getFullUrl())) {
                count = uniqueUrlCounts.get(nav.getFullUrl());
            } else {
                count = new Long(0L);
            }
            uniqueUrlCounts.put(nav.getFullUrl(), count + 1l);
        }
        return uniqueUrlCounts;
    }
}
