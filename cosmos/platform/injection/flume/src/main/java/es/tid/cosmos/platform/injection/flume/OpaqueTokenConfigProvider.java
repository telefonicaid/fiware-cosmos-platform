package es.tid.cosmos.platform.injection.flume;

import java.util.Properties;
import java.util.UUID;

/**
 * The interface of any object capable of providing configuration parameters
 * for an opaque token.
 *
 * @author apv
 */
public interface OpaqueTokenConfigProvider {

    OpaqueTokenConfig forToken(UUID opaqueToken);

    /**
     * The interface of any object able to build new instances of
     * {@link OpaqueTokenConfigProvider}
     */
    interface Builder {

        OpaqueTokenConfigProvider newConfigProvider(Properties props);
    }

}
