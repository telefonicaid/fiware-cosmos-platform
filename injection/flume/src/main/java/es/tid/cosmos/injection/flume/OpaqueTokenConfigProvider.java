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

import java.util.Properties;
import java.util.UUID;

/**
 * The interface of any object capable of providing configuration parameters
 * for an opaque token.
 *
 * @author apv
 */
public interface OpaqueTokenConfigProvider {

    /**
     * Obtains configuration for given opaque token. If no config was found,
     * return null.
     * @param opaqueToken the opaque token which configuration is to be
     *                    obtained.
     * @return the opaque token configuration or null if given token is
     * unknown.
     */
    OpaqueTokenConfig forToken(UUID opaqueToken);

    /**
     * The interface of any object able to build new instances of
     * {@link OpaqueTokenConfigProvider}
     */
    interface Builder {

        OpaqueTokenConfigProvider newConfigProvider(Properties props);
    }

}
