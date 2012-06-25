package es.tid.smartsteps.flume;

import java.util.Map;
import java.util.UUID;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.interceptor.Interceptor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A Flume interceptor that manages the opaque token of Cosmos's data injection
 * service.
 *
 * @author apv
 */
public class OpaqueTokenProcessorInterceptor extends AbstractInterceptor {

    private static final Logger LOGGER =
            LoggerFactory.getLogger(OpaqueTokenProcessorInterceptor.class);
    private static final String HEADER_OPAQUE_TOKEN = "cosmos-opaque-token";

    private final UUID opaqueToken;

    public OpaqueTokenProcessorInterceptor(UUID opaqueToken) {
        this.opaqueToken = opaqueToken;
    }

    @Override
    public Event intercept(Event event) {
        Map<String, String> headers = event.getHeaders();
        String prevValue = headers.get(HEADER_OPAQUE_TOKEN);
        if (prevValue != null) {
            LOGGER.error(String.format("header '%s' already present in " +
                    "intercepted event with value '%s'; discarding due to " +
                    "illegal manipulation of reserved header",
                    HEADER_OPAQUE_TOKEN, prevValue));
            return null;
        }
        headers.put(HEADER_OPAQUE_TOKEN, this.opaqueToken.toString());
        return event;
    }

    public static class Builder implements Interceptor.Builder {

        private static final String PROPERTY_OPAQUE_TOKEN = "token";

        private UUID opaqueToken = null;

        @Override
        public Interceptor build() {
            if (this.opaqueToken == null) {
                throw new IllegalStateException("build() called on a " +
                        "non-configured interceptor builder");
            }
            return new OpaqueTokenProcessorInterceptor(opaqueToken);
        }

        @Override
        public void configure(Context context) {
            String strTk = context.getString(PROPERTY_OPAQUE_TOKEN);
            if (strTk == null) {
                throw new ConfigurationException(String.format(
                        "missing '%s' property for opaque token " +
                        "processor interceptor", PROPERTY_OPAQUE_TOKEN));
            }
            try {
                this.opaqueToken = UUID.fromString(strTk);
            } catch (IllegalArgumentException e) {
                throw new ConfigurationException(String.format(
                        "invalid value '%s' for property '%s': not a UUID",
                        strTk, PROPERTY_OPAQUE_TOKEN), e);
            }
        }
    }
}
