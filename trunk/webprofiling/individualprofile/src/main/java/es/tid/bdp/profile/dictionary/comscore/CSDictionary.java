package es.tid.bdp.profile.dictionary.comscore;

import java.io.IOException;
import java.net.URI;

import es.tid.bdp.profile.dictionary.Categorization;
import es.tid.bdp.profile.dictionary.CategorizationResult;
import es.tid.bdp.profile.dictionary.Dictionary;

/*
 * Class to handle the comScore dictionary.
 *
 * @author dmicol, sortega
 */
public class CSDictionary implements Dictionary {
    public static final String DEFAULT_COMSCORE_LIB =
            "/opt/hadoop/lib/native/Linux-amd64-64/libcomscore.so";

    private boolean isInitialized;
    private final String dictionayFile;
    private CSDictionaryJNIInterface dictionary;

    public CSDictionary(String dictionayFile) {
        this.isInitialized = false;
        this.dictionayFile = dictionayFile;
    }

    @Override
    public void init() throws IOException {
        if (this.isInitialized) {
            return;
        }

        System.setProperty(CSDictionaryJNIInterface.COMSCORE_LIB_PROPERTY,
                DEFAULT_COMSCORE_LIB);
        this.dictionary = new CSDictionaryJNIInterface();
        this.dictionary.loadCSDictionary(dictionayFile);
        this.isInitialized = true;
    }

    @Override
    public Categorization categorize(String url) {
        if (!this.isInitialized) {
            throw new IllegalStateException(
                    "Cannot categorize prior to dictionary initialization.");
        }

        URI uri = URI.create(url);
        String normalizedUrl = uri.getHost() + uri.getPath();
        int[] categories = this.dictionary.lookupCategories(normalizedUrl);
        if (categories.length == 0) {
            return this.processUknownUrl();
        } else {
            return this.processKnownUrl(categories);
        }
    }

    private Categorization processKnownUrl(int[] categoryIds) {
        Categorization categorization = new Categorization();

        String[] categoryNames = new String[categoryIds.length];
        for (int i = 0; i < categoryIds.length; i++) {
            categoryNames[i] = this.dictionary.getCategoryName(
                    categoryIds[i]);
        }
        categorization.setResult(CategorizationResult.KNOWN_URL);
        categorization.setCategories(categoryNames);
        return categorization;
    }

    private Categorization processUknownUrl() {
        Categorization categorization = new Categorization();
        categorization.setResult(CategorizationResult.UNKNOWN_URL);
        return categorization;
    }
}
