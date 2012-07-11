package es.tid.cosmos.platform.injection.flume;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.UUID;

/**
 * The opaque token cache provides a cache of UUID objects mapped by
 * its string representation. This may be used to avoid using
 * {@link java.util.UUID#fromString(String)} repeated times, which causes a high
 * impact in the CPU performance.
 *
 * @author apv
 */
public class OpaqueTokenCache {

    public static final int DEFAULT_CAPACITY = 512;

    private int capacity;
    private HashMap<String, UUID> tokens;

    public OpaqueTokenCache() {
        this(DEFAULT_CAPACITY);
    }

    public OpaqueTokenCache(int capacity) {
        this.capacity = capacity;
        this.tokens = new LinkedHashMap<String, UUID>(capacity) {
            @Override
            protected boolean removeEldestEntry(Map.Entry<String, UUID> e) {
                return this.size() > OpaqueTokenCache.this.capacity;
            }
        };
    }

    public int getCapacity() {
        return this.capacity;
    }

    public int getSize() {
        return this.tokens.size();
    }

    /**
     * Fetch the given stringfied opaque token from this cache. If a cache
     * success occurs, it returns the cached token in UUID format. If a cache
     * failure occurs, the stringfied token is transformed into UUID format,
     * inserted in the cache and then it's returned.
     * @param stringfiedOpaqueToken the stringfied opaque token to be fetched
     * @return the UUID opaque token either fetched or justly inserted
     * @throws IllegalArgumentException if given argument is not a valid
     * stringfied opaque token.
     */
    public UUID fetch(String stringfiedOpaqueToken) {
        UUID result = this.tokens.get(stringfiedOpaqueToken);
        if (result == null) {
            result = UUID.fromString(stringfiedOpaqueToken);
            this.tokens.put(stringfiedOpaqueToken, result);
        }
        return result;
    }

}
