package es.tid.cosmos.platform.injection.flume;

/**
 * A POJO with config properties of an opaque token.
 *
 * @author apv
 */
public final class OpaqueTokenConfig {

    private String transformation;
    private String destinationPath;

    public String getTransformation() {
        return transformation;
    }

    public String getDestinationPath() {
        return destinationPath;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    private OpaqueTokenConfig() {
        this.transformation = null;
        this.destinationPath = null;
    }

    /**
     * Validate the config object checking the completeness of the properties.
     *
     * @throws IllegalStateException when token config is not completed
     */
    private void validate() {
        if (this.transformation == null) {
            throw new IllegalStateException("missing value for transformation" +
                    " property in opaque token config object");
        }
        if (this.destinationPath == null) {
            throw new IllegalStateException("missing value for destinationPath" +
                    " property in opaque token config object");
        }
    }

    /**
     * Fluent builder class for {@link OpaqueTokenConfig}.
     */
    public static final class Builder {

        private final OpaqueTokenConfig config;

        private Builder() {
            this.config = new OpaqueTokenConfig();
        }

        public Builder withTransformation(String transformation) {
            this.config.transformation = transformation;
            return this;
        }

        public Builder withDestinationPath(String destinationPath) {
            this.config.destinationPath = destinationPath;
            return this;
        }

        /**
         * Build the opaque token config.
         *
         * @return the resulting opaque token config.
         * @throws IllegalStateException when there are uninitialized
         * properties
         */
        public OpaqueTokenConfig build() {
            this.config.validate();
            return this.config;
        }
    }
}
