package es.tid.bdp.profile.dictionary.comscore;

import java.io.File;
import java.util.Arrays;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class CSDictionaryJNIInterfaceTest {
    private static final String TEST_DICTIONARY =
            "src/test/resources/dictionary.bin";
    private static final String[] LIB_EXTENSIONS = {"so", "dylib"};
    private static final int THREAD_COUNT = 4;
    private static final int TEST_COUNT = 10000;
    private CSDictionaryJNIInterface instance;

    @BeforeClass
    public static void setUpClass() {
        File library;
        for (String ext : LIB_EXTENSIONS) {
            library = new File("src/main/cpp/libcomscore." + ext);
            if (library.exists()) {
                System.setProperty(
                        CSDictionaryJNIInterface.COMSCORE_LIB_PROPERTY,
                        library.getAbsolutePath());
                return;
            }
        }
        fail("Native library not found");
    }

    @Before
    public void setUp() throws Exception {
        this.instance = new CSDictionaryJNIInterface();
        assertTrue("Cannot initialize dictionary",
                instance.loadCSDictionary(TEST_DICTIONARY));
    }

    @Test
    public void testJNIInterface() throws Exception {
        int[] categories = instance.lookupCategories(
                "www.newsalert.com/user/123");
        assertEquals(2, categories.length);
        assertEquals("Business/Finance",
                instance.getCategoryName(categories[0]));
        assertEquals("News/Research", instance.getCategoryName(categories[1]));

        assertEquals(0, instance.lookupCategories("unknown.org").length);
    }

    private static class DictionaryWorker extends Thread {
        public boolean success;
        private final CSDictionaryJNIInterface dictionary;

        public DictionaryWorker(CSDictionaryJNIInterface dictionary) {
            this.dictionary = dictionary;
        }

        @Override
        public void run() {
            for (int test = 0; test < TEST_COUNT; ++test) {
                int[] categories = dictionary.lookupCategories("ASKJEEVES.COM"
                        + "/web?o=10181&jr=true&q=hola&qsrc=0&o=10181&l=dir");
                assertEquals("Search/Navigation",
                        dictionary.getCategoryName(categories[0]));
            }
            this.success = true;
        }
    }

    @Test(timeout=1500)
    public void testConcurrentUse() throws Exception {
        DictionaryWorker[] workers = new DictionaryWorker[THREAD_COUNT];
        for (int w = 0; w < THREAD_COUNT; w++) {
            workers[w] = new DictionaryWorker(instance);
            workers[w].start();
        }

        for (DictionaryWorker worker : workers) {
            worker.join();
            assertTrue(worker.success);
        }
    }

    @Test
    public void testGetAllCategories() throws Exception {
        String[] names = instance.getAllCategoryNames();
        assertEquals(132, names.length);
        assertArrayEquals(new String[] {"Auctions", "Automotive",
                "Business/Finance", "Career Services and Development",
                "Community"}, Arrays.copyOfRange(names, 0, 5));
    }
}
