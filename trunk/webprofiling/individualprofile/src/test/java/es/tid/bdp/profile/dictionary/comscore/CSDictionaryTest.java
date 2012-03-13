package es.tid.bdp.profile.dictionary.comscore;

import org.junit.Test;

/**
 *
 * @author sortega
 */
public class CSDictionaryTest {
    @Test(expected = IllegalStateException.class)
    public void shouldRequireInitialization() {
        new CSDictionary(
                "terms",
                "dictionary",
                "categoryPatternMapping",
                "categoryNames",
                new String[] {}).categorize("http://host/path");
    }
}
