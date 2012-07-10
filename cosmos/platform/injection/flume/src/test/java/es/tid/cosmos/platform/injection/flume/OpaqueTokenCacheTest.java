package es.tid.cosmos.platform.injection.flume;

import java.util.UUID;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * @author apv
 */
public class OpaqueTokenCacheTest {

    private OpaqueTokenCache cache;

    @Before
    public void setup() {
        this.cache = new OpaqueTokenCache(4);
    }

    @Test
    public void testCacheFailure() {
        UUID tk1 = UUID.randomUUID();
        UUID tk2 = this.cache.fetch(tk1.toString());
        assertEquals(tk1, tk2);
        assertEquals(1, this.cache.size());
    }

    @Test
    public void testCacheSuccess() {
        UUID tk1 = UUID.randomUUID();
        this.cache.fetch(tk1.toString()); // failure, fill cache
        UUID tk2 = this.cache.fetch(tk1.toString()); // success
        assertEquals(tk1, tk2);
        assertEquals(1, this.cache.size());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testInvalidFetch() {
        this.cache.fetch("Hello world!");
    }

    @Test
    public void testReplacement() {
        for (int i = 0; i < this.cache.capacity() + 1; i++) {
            this.cache.fetch(UUID.randomUUID().toString());
        }
        assertEquals(this.cache.capacity(), this.cache.size());
    }
}
