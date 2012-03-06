package es.tid.bdp.profile.dictionary.comscore;

import org.apache.hadoop.fs.Path;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class CSDictionaryTest {
    
    @Test(expected = IllegalStateException.class)
    public void shouldRequireInitialization() {
        new CSDictionary(new Path[] {}).categorize("http://host/path");
    }
}
