package es.tid.bdp.profile.dictionary;

import java.io.IOException;

/**
 * Interaface for the URL categorization dictionaries.
 * 
 * @author dmicol
 */
public interface Dictionary {
    void init() throws IOException;
    Categorization categorize(String url);
}