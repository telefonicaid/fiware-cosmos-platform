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
 * A class that defines the constants corresponding to opaque token management.
 */
public abstract class OpaqueTokenConstants {

    /**
     * The name of Flume event header which determines the opaque token
     */
    public static final String EVENT_HEADER_OPAQUE_TOKEN = "cosmos-opaque-token";

    /**
     * The name of Flume event header which determines the transformation to
     * be applied
     */
    public static final String EVENT_HEADER_TRANSFORMATION =
            "cosmos-transformation";

    /**
     * The name of Flume event header which determines the destination path
     * in the datastore.
     */
    public static final String EVENT_HEADER_DESTINATION_PATH =
            "cosmos-destination-path";

}
