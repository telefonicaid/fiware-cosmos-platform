package es.tid.cosmos.profile.dictionary.comscore;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.Collection;

import es.tid.cosmos.profile.dictionary.Categorization;
import es.tid.cosmos.profile.dictionary.CategorizationResult;
import es.tid.cosmos.profile.dictionary.Dictionary;

/*
 * Class to handle the comScore dictionary.
 *
 * @author dmicol, sortega
 */
public class CSDictionary implements Dictionary {
    private static Class<? extends NativeCSDictionary>
            dictionaryInterfaceClass = CSDictionaryJNIInterface.class;

    private boolean isInitialized;
    private NativeCSDictionary dictionary;
    private CSPatternToCategoryMap patternToCategoryMap;
    private CSCategoryIdToNameMap categoryIdToNameMap;
    private final String[] nativeLibraries;

    private final String termsFileName;
    private final String dictionaryFileName;
    private final String categoryPatternMappingFileName;
    private final String categoryNamesFileName;

    public static void setNativeInterfaceClass(Class<? extends
            NativeCSDictionary> clazz) {
        dictionaryInterfaceClass = clazz;
    }

    public CSDictionary(String termsFileName,
            String dictionaryFileName,
            String categoryPatternMappingFileName,
            String categoryNamesFileName,
            String[] nativeLibraries) {
        this.isInitialized = false;
        this.termsFileName = termsFileName;
        this.dictionaryFileName = dictionaryFileName;
        this.categoryPatternMappingFileName = categoryPatternMappingFileName;
        this.categoryNamesFileName = categoryNamesFileName;
        this.nativeLibraries = nativeLibraries.clone();
    }

    @Override
    public void init() throws IOException {
        if (this.isInitialized) {
            return;
        }
        try {
            for (String nativeLibrary : this.nativeLibraries) {
                System.load(nativeLibrary);
            }
            this.dictionary = dictionaryInterfaceClass.newInstance();
        } catch (Exception ex) {
            throw new IllegalStateException("Misconfigured native dictionary "
                    + "interface", ex);
        }
        this.dictionary.loadCSDictionary(NativeCSDictionary.DEFAULT_MODE,
                this.termsFileName, this.dictionaryFileName);
        this.loadCategoryPatternMapping(this.categoryPatternMappingFileName);
        this.loadCategoryNames(this.categoryNamesFileName);
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
        long patternId = this.dictionary.applyDictionaryUsingUrl(normalizedUrl);
        if (patternId < 0) {
            return this.processIrrelevantUrl();
        } else if (patternId == 0) {
            throw new IllegalArgumentException("Invalid pattern ID.");
        } else if (patternId == 1) {
            return this.processUknownUrl();
        } else {
            return this.processKnownUrl(patternId);
        }
    }

    @Override
    public String[] getAllCategoryNames() {
        Collection<String> categoriesCollection =
                this.categoryIdToNameMap.getCategories();
        String[] categories = new String[categoriesCollection.size()];
        return categoriesCollection.toArray(categories);
    }

    private Categorization processKnownUrl(long patternId) {
        Categorization categorization = new Categorization();
        long[] categoryIds;
        try {
            categoryIds = this.patternToCategoryMap.getCategories(patternId);
        } catch (IllegalArgumentException ex) {
            categorization.setResult(CategorizationResult.GENERIC_FAILURE);
            return categorization;
        }

        String[] categoryNames = new String[categoryIds.length];
        for (int i = 0; i < categoryIds.length; i++) {
            categoryNames[i] = this.categoryIdToNameMap.getCategoryName(
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

    private Categorization processIrrelevantUrl() {
        Categorization categorization = new Categorization();
        categorization.setResult(CategorizationResult.IRRELEVANT_URL);
        return categorization;
    }

    private void loadCategoryPatternMapping(String fileName)
            throws IOException {
        this.patternToCategoryMap = new CSPatternToCategoryMap();

        FileInputStream file = new FileInputStream(fileName);
        InputStreamReader input = new InputStreamReader(file);
        this.patternToCategoryMap.init(input);
        file.close();
    }

    private void loadCategoryNames(String fileName) throws IOException {
        this.categoryIdToNameMap = new CSCategoryIdToNameMap();

        FileInputStream file = new FileInputStream(fileName);
        InputStreamReader input = new InputStreamReader(file);
        this.categoryIdToNameMap.init(input);
        file.close();
    }
}
