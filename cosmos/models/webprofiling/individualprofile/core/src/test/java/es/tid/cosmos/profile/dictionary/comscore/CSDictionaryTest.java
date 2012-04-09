package es.tid.cosmos.profile.dictionary.comscore;

import static java.util.Arrays.asList;

import static org.hamcrest.Matchers.hasItems;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.profile.dictionary.Categorization;
import es.tid.cosmos.profile.dictionary.CategorizationResult;

/**
 *
 * @author sortega
 */
public class CSDictionaryTest {
    private static final String RES_PATH = "src/test/resources/";
    private CSDictionary instance;

    public static class FakeNativeInterface implements NativeCSDictionary {
        public static String loadedDictionary = null;
        public static int initializations;

        public FakeNativeInterface() {
            initializations = 0;
        }

        @Override
        public long applyDictionaryUsingUrl(String szURL) {
            if (szURL.contains("walkthrough")) {
                return 13256251L;
            } else if (szURL.contains("generic-failure")) {
                return 666L;
            } else if (szURL.contains("unknown")) {
                return 1L;
            } else if (szURL.contains("invalid")) {
                return 0L;
            } else {
                return -1L;
            }
        }

        @Override
        public boolean initFromTermsInDomainFlatFile(int iMode,
                String szTermsInDomainFlatFileName) {
            return true;
        }

        @Override
        public boolean loadCSDictionary(int iMode,
                String szTermsInDomainFlatFileName, String szDictionaryName) {
            loadedDictionary = szDictionaryName;
            initializations++;
            return true;
        }
    }

    @Before
    public void setUp() throws Exception {
        this.instance = new CSDictionary(
                RES_PATH + "terms.txt",
                RES_PATH + "dictionary.txt",
                RES_PATH + "categoryPatternMapping.txt",
                RES_PATH + "categoryNames.txt",
                new String[] {});
        CSDictionary.setNativeInterfaceClass(
                CSDictionaryTest.FakeNativeInterface.class);
    }

    @Test(expected = IllegalStateException.class)
    public void shouldRequireInitialization() {
        instance.categorize("http://host/path");
    }

    @Test
    public void shouldReadFilesInInitialization() throws Exception {
        instance.init();
        assertEquals(RES_PATH + "dictionary.txt",
                FakeNativeInterface.loadedDictionary);
    }

    @Test
    public void shouldAvoidMultipleInitializations() throws Exception {
        instance.init();
        assertEquals(1, FakeNativeInterface.initializations);
        instance.init();
        assertEquals(1, FakeNativeInterface.initializations);
    }

    @Test
    public void shouldCategorizeKnownPattern() throws Exception {
        instance.init();
        Categorization result = instance.categorize("www.walkthroughgame.org");
        assertEquals(CategorizationResult.KNOWN_URL, result.getResult());
        assertThat(asList(result.getCategories()), hasItems("Games",
                "Online Gaming"));
    }

    @Test
    public void shouldCategorizeIrrelevantPatterns() throws Exception {
        instance.init();
        assertEquals(CategorizationResult.IRRELEVANT_URL,
                instance.categorize("irrelevant-domain.com").getResult());
    }

    @Test
    public void shouldCategorizeGenericFailures() throws Exception {
        instance.init();
        assertEquals(CategorizationResult.GENERIC_FAILURE,
                instance.categorize("www.generic-failure.gov").getResult());
    }

    @Test
    public void shouldCategorizeUnknownDomains() throws Exception {
        instance.init();
        assertEquals(CategorizationResult.UNKNOWN_URL,
                instance.categorize("unknown.org/domain").getResult());
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldThrowExceptionOnInvalidPattern() throws Exception {
        instance.init();
        instance.categorize("invalid-patterm.com");
    }

    @Test
    public void shouldGetAllCategoryNames() throws Exception {
        instance.init();
        String[] names = instance.getAllCategoryNames();
        assertEquals(132, names.length);
        assertThat(asList(names), hasItems("Games", "Online Gaming", "Sports",
                "XXX Adult", "Health"));
    }
}
