package es.tid.bdp.profile.dictionary.comscore;

import java.io.StringReader;
import static org.junit.Assert.assertArrayEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class CSPatternToCategoryMapTest {
    private CSPatternToCategoryMap instance;
    private StringReader input;
    
    @Before
    public void setUp() throws Exception {
        this.instance = new CSPatternToCategoryMap();
        this.input = new StringReader("664169\n"
                + "670943\n"
                + "664169	778308	778309\n"
                + "664277	778308	3322210\n"
                + "666285	778308	3322210\n"
                + "670934	778213	778230	778314\n"
                + "670935	778227	778231\n"
                + "670937	778215	778248\n"
                + "670942	778227	778231\n"
                + "670943	778218	778230	778315\n");
        this.instance.init(this.input);
    }

    @Test
    public void shouldFindKnownMapping() throws Exception {
        assertArrayEquals(new long[]{ 778308l, 3322210l },
                this.instance.getCategories(666285l));
    }
    
    @Test(expected=IllegalArgumentException.class)
    public void shouldFailForUnknownPatterns() throws Exception {
        this.instance.getCategories(555555l);
    }
}
