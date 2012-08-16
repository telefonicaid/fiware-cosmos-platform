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
