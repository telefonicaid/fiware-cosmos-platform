package es.tid.cosmos.platform.injection.flume;

import java.util.Properties;
import java.util.UUID;

/**
 * An opaque token config provided that returns a static configuration for
 * a given token.
 */
public final class StaticOpaqueTokenConfigProvider implements
        OpaqueTokenConfigProvider {

    private final UUID token;
    private final String transformation;
    private final String destination;

    private StaticOpaqueTokenConfigProvider(
            UUID token, String transformation, String destination) {
        this.token = token;
        this.transformation = transformation;
        this.destination = destination;
    }

    @Override
    public OpaqueTokenConfig forToken(UUID opaqueToken) {
        return (this.token.equals(opaqueToken)) ?
                OpaqueTokenConfig.newBuilder()
                        .withTransformation(this.transformation)
                        .withDestinationPath(this.destination)
                        .build() : null;
    }

    public static final class Builder implements
            OpaqueTokenConfigProvider.Builder {

        public static final String PROPERTY_TOKEN = "token";
        public static final String PROPERTY_TRANSFORMATION = "transformation";
        public static final String PROPERTY_DESTINATION = "destination";

        @Override
        public StaticOpaqueTokenConfigProvider newConfigProvider(Properties props) {
            String token = readPropertyNotNull(props, PROPERTY_TOKEN);
            String transformation = readPropertyNotNull(props,
                    PROPERTY_TRANSFORMATION);
            String destination = readPropertyNotNull(props,
                    PROPERTY_DESTINATION);

            try {
                return new StaticOpaqueTokenConfigProvider(
                        UUID.fromString(token), transformation, destination);
            } catch (IllegalArgumentException e) {
                throw new IllegalArgumentException(String.format(
                        "invalid given value '%s' for '%s' property: not a " +
                                "valid opaque token", token, PROPERTY_TOKEN), e);
            }
        }

        private String readPropertyNotNull(Properties properties,
                                         String property) {
            String value = properties.getProperty(property);
            if (value == null) {
                throw new IllegalArgumentException(String.format(
                        "missing '%s' property for opaque token config provider",
                        property));
            }
            return value;
        }
    }
}
