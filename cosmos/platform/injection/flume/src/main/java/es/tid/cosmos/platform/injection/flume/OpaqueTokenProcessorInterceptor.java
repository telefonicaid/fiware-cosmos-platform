package es.tid.cosmos.platform.injection.flume;

import java.util.Map;
import java.util.Properties;
import java.util.UUID;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.interceptor.Interceptor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A Flume interceptor to process opaque tokens as part of boundary agent.
 *
 * @author apv
 */
public final class OpaqueTokenProcessorInterceptor extends
        AbstractInterceptor {

    private static final Logger LOGGER =
            LoggerFactory.getLogger(OpaqueTokenProcessorInterceptor.class);

    private final OpaqueTokenConfigProvider configProvider;

    private OpaqueTokenProcessorInterceptor(
            OpaqueTokenConfigProvider configProvider) {
        this.configProvider = configProvider;
    }

    @Override
    public Event intercept(Event event) {
        String strTk = event.getHeaders().get(
                OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN);
        if (strTk == null) {
            LOGGER.warn(String.format("missing '%s' header in incoming " +
                    "event; event discarded",
                    OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN));
            return null;
        }
        UUID token;
        try {
            token = UUID.fromString(strTk);
        } catch (IllegalArgumentException e) {
            LOGGER.warn(String.format("cannot parse opaque token from " +
                    "value '%s' obtained from '%s' header; event discarded",
                    strTk, OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN),
                    e);
            return null;
        }
        OpaqueTokenConfig config = this.configProvider.forToken(token);

        Map<String, String> headers = event.getHeaders();
        headers.put(OpaqueTokenConstants.EVENT_HEADER_TRANSFORMATION,
                config.getTransformation());
        headers.put(OpaqueTokenConstants.EVENT_HEADER_DESTINATION_PATH,
                config.getDestinationPath());
        return event;
    }

    public static final class Builder implements Interceptor.Builder {

        private OpaqueTokenConfigProvider configProvider;

        public static final String PROPERTY_TOKEN_CONFIG_PROVIDER =
                "token-config-provider";

        @Override
        public OpaqueTokenProcessorInterceptor build() {
            return new OpaqueTokenProcessorInterceptor(this.configProvider);
        }

        @Override
        public void configure(Context context) {
            String configProviderName = context.getString(
                    PROPERTY_TOKEN_CONFIG_PROVIDER);
            if (configProviderName == null) {
                throw new ConfigurationException(String.format(
                        "in opaque token processor interceptor: missing '%s' " +
                                "property", PROPERTY_TOKEN_CONFIG_PROVIDER));
            }
            OpaqueTokenConfigProvider.Builder builder;
            try {
                builder = OpaqueTokenConfigProviderType.valueOf
                        (configProviderName).getBuilder();
            } catch (IllegalArgumentException e) {
                builder = builderForName(configProviderName);
            }
            try {
                Properties props = new Properties();
                props.putAll(context.getSubProperties(
                        PROPERTY_TOKEN_CONFIG_PROVIDER + "."));
                this.configProvider = builder.newConfigProvider(props);
            } catch (IllegalArgumentException e) {
                throw new ConfigurationException(String.format(
                        "in opaque token processor interceptor: missing " +
                                "sub-properties under '%s' property",
                        PROPERTY_TOKEN_CONFIG_PROVIDER), e);
            }
        }

        private OpaqueTokenConfigProvider.Builder builderForName(String className) {
            Class<? extends OpaqueTokenConfigProvider.Builder> builderClass;
            try {
                Class clazz = ClassLoader.getSystemClassLoader().loadClass(
                        className);
                if (!OpaqueTokenConfigProvider.Builder.class.isAssignableFrom(
                        clazz)) {
                    throw new ConfigurationException(String.format("in opaque " +
                            "token processor interceptor: given builder class " +
                            "'%s' in '%s' property is not a valid builder",
                            className,
                            PROPERTY_TOKEN_CONFIG_PROVIDER));
                }
                builderClass = clazz;
            } catch (ClassNotFoundException e) {
                throw new ConfigurationException(String.format("in opaque " +
                        "token processor interceptor: unknown token " +
                        "config provider or builder class for name '%s' in " +
                        "'%s' property", className,
                        PROPERTY_TOKEN_CONFIG_PROVIDER), e);
            }
            try {
                return builderClass.newInstance();
            } catch (Exception e) {
                throw new ConfigurationException(String.format("in opaque " +
                        "token processor interceptor: given builder class " +
                        "'%s' in '%s' property cannot be instantiated",
                        className,
                        PROPERTY_TOKEN_CONFIG_PROVIDER), e);
            }
        }
    }
}
