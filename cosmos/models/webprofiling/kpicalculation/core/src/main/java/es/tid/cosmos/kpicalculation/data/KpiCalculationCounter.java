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

package es.tid.cosmos.kpicalculation.data;

/**
 * Enum in where define the counters that we are going to monitor into the
 * cluster. <br>
 * LINE_FILTERED_EXTENSION: Number of lines filtered by its extension <br>
 * LINE_FILTERED_3RDPARTY: Number of lines filtered because of belonging to 3rd
 * Party domains <br>
 * LINE_FILTERED_PERSONAL_INFO: Number of lines filtered because of belonging to
 * Personal Info domains WRONG_FILTER_CONFIGURATION: Number of wrong configured
 * filters
 *
 * @author javierb
 */
public enum KpiCalculationCounter {
    LINE_STORED,
    LINE_FILTERED_EXTENSION,
    LINE_FILTERED_3RDPARTY,
    LINE_FILTERED_PERSONAL_INFO,
    WRONG_FILTER_CONFIGURATION,
    WRONG_FILTERING_FIELDS,
    MALFORMED_URL,
    WRONG_LINE_FORMAT,
    COMPARATION_ERROR
}