package es.tid.bdp.profile.dictionary.comscore;

import java.io.File;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.profile.dictionary.Categorization;
import es.tid.bdp.profile.dictionary.CategorizationResult;

/**
 *
 * @author sortega
 */
public class CSDictionaryTest {
    private static final String TEST_DICTIONARY =
            "src/test/resources/dictionary.bin";
    private static final String[] LIB_EXTENSIONS = { "so", "dylib" };
    
    private CSDictionary instance;

    @Before
    public void setUp() throws Exception {
        // TODO: should reuse from CSDictionaryJNIInterfaceTest
        File library;
        String comscoreLib = null;
        for (String ext : LIB_EXTENSIONS) {
            library = new File("src/main/cpp/libcomscore." + ext);
            if (library.exists()) {
                comscoreLib = library.getAbsolutePath();
                break;
            }
        }
        this.instance = new CSDictionary(TEST_DICTIONARY, comscoreLib);
        this.instance.init();
    }
    
    @Test(expected = IllegalStateException.class)
    public void shouldRequireInitialization() {
        new CSDictionary("/dict.bin", "").categorize("http://host/path");
    }
    
    @Test
    public void shouldBeKnownUrl() {
        Categorization categorization = this.instance.categorize(
                "www.newsalert.com/user/123");
        assertEquals(CategorizationResult.KNOWN_URL,
                     categorization.getResult());
    }

    @Test
    public void shouldBeUnknownUrl() {
        Categorization categorization = this.instance.categorize(
                "www.mutxamel.org");
        assertEquals(CategorizationResult.UNKNOWN_URL,
                     categorization.getResult());
    }
}
