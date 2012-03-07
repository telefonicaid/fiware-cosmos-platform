package es.tid.bdp.profile.dictionary.comscore;

import org.junit.Test;

/**
 *
 * @author sortega
 */
public class CSDictionaryTest {

    @Test(expected = IllegalStateException.class)
    public void shouldRequireInitialization() {
        new CSDictionary("/dict.bin").categorize("http://host/path");
    }
}
