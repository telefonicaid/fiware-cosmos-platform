package es.tid.cosmos.platform.injection.flume;

/**
 * An enumeration of the known opaque token config providers.
 */
public enum OpaqueTokenConfigProviderType {

    STATIC(new StaticOpaqueTokenConfigProvider.Builder());

    /**
     * Obtains the builder class of the represented token config provider.
     */
    public OpaqueTokenConfigProvider.Builder getBuilder() {
        return this.builder;
    }

    private OpaqueTokenConfigProvider.Builder builder;

    private OpaqueTokenConfigProviderType(
            OpaqueTokenConfigProvider.Builder builder) {
        this.builder = builder;
    }
}
