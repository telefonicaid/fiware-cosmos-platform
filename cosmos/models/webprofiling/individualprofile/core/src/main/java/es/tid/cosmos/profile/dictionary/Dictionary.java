package es.tid.cosmos.profile.dictionary;

import java.io.IOException;

/**
 * Interaface for the URL categorization dictionaries.
 *
 * @author dmicol, sortega
 */
public interface Dictionary {
    void init() throws IOException;
    Categorization categorize(String url);
    String[] getAllCategoryNames();
}
