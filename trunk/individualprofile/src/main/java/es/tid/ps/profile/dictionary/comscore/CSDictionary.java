package es.tid.ps.profile.dictionary.comscore;

import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.CategorizationResult;
import es.tid.ps.profile.dictionary.Dictionary;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.Arrays;
import java.util.List;
import org.apache.hadoop.fs.Path;

/*
 * Class to handle the comScore dictionary.
 *
 * @author dmicol
 */
public class CSDictionary implements Dictionary {
    private static final String TERMS_IN_DOMAIN_FILENAME =
            "cs_terms_in_domain.bcp";
    private static final String DICTIONARY_FILENAME =
            "cs_mmxi_143.bcp.gz";
    private static final String CATEGORY_PATTERN_MAPPING_FILENAME =
            "patterns_to_categories.txt";
    private static final String CATEGORY_NAMES_FILENAME =
            "cat_subcat_lookup_143m.txt";
    
    private boolean isInitialized;
    private final List<Path> dictionayFiles;
    private CSDictionaryJNIInterface dictionary;
    private CSPatternToCategoryMap patternToCategoryMap;
    private CSCategoryIdToNameMap categoryIdToNameMap;

    public CSDictionary(Path[] dictionayFiles) {
        this.isInitialized = false;
        this.dictionayFiles = Arrays.asList(dictionayFiles);
    }

    @Override
    public void init() throws IOException {
        if (this.isInitialized) {
            return;
        }

        String termsInDomainFlatFileName = null;
        String dictionaryFileName = null;
        String categoryPatterMappingFileName = null;
        String categoryNamesFileName = null;

        for (Path path : this.dictionayFiles) {
            if (path.getName().equals(TERMS_IN_DOMAIN_FILENAME)) {
                termsInDomainFlatFileName = path.toString();
            } else if (path.getName().equals(DICTIONARY_FILENAME)) {
                dictionaryFileName = path.toString();
            } else if (path.getName().equals(
                    CATEGORY_PATTERN_MAPPING_FILENAME)) {
                categoryPatterMappingFileName = path.toString();
            } else if (path.getName().equals(CATEGORY_NAMES_FILENAME)) {
                categoryNamesFileName = path.toString();
            }
        }

        this.dictionary = new CSDictionaryJNIInterface();
        this.dictionary.LoadCSDictionary(1, termsInDomainFlatFileName,
                dictionaryFileName);
        this.loadCategoryPatternMapping(categoryPatterMappingFileName);
        this.loadCategoryNames(categoryNamesFileName);
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
        long patternId = this.dictionary.ApplyDictionaryUsingUrl(normalizedUrl);
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

    private Categorization processKnownUrl(long patternId) {
        Categorization categorization = new Categorization();
        long[] categoryIds;
        try {
            categoryIds = this.patternToCategoryMap.getCategories(patternId);
        } catch (IllegalArgumentException ex) {
            categorization.result = CategorizationResult.GENERIC_FAILURE;
            return categorization;
        }

        String[] categoryNames = new String[categoryIds.length];
        for (int i = 0; i < categoryIds.length; i++) {
            categoryNames[i] = this.categoryIdToNameMap.getCategoryName(
                    categoryIds[i]);
        }
        categorization.result = CategorizationResult.KNOWN_URL;
        categorization.categories = categoryNames;
        return categorization;
    }

    private Categorization processUknownUrl() {
        Categorization categorization = new Categorization();
        categorization.result = CategorizationResult.UNKNOWN_URL;
        return categorization;
    }

    private Categorization processIrrelevantUrl() {
        Categorization categorization = new Categorization();
        categorization.result = CategorizationResult.IRRELEVANT_URL;
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
