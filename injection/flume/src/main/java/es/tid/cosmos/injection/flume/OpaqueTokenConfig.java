/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.injection.flume;

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

        /**
         * Set the transformation to be applied to the event data. This
         * field is optional and defaulted to null.
         */
        public Builder withTransformation(String transformation) {
            this.config.transformation = transformation;
            return this;
        }

        /**
         * Set the destination path in datastore to persist the event data.
         * This field is mandatory. If missing, the {@link #build()}
         * operation will raise a {@link IllegalStateException}.
         * @param destinationPath
         * @return
         */
        public Builder withDestinationPath(String destinationPath) {
            this.config.destinationPath = destinationPath;
            return this;
        }

        /**
         * Build the opaque token config.
         *
         * @return the resulting opaque token config.
         * @throws IllegalStateException when one or more mandatory properties
         * are uninitialized.
         */
        public OpaqueTokenConfig build() {
            this.config.validate();
            return this.config;
        }
    }
}
