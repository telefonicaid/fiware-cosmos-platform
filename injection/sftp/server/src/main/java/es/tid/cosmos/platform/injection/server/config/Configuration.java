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

package es.tid.cosmos.platform.injection.server.config;

import java.net.URI;
import java.net.URL;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;

/**
 *
 * @author sortega
 */
public class Configuration {
    private static final String DB_NAME = "DB_NAME";
    private static final String DB_PASSWORD = "DB_PASS";
    private static final String DB_USER = "DB_USER";
    private static final String FRONTEND_DB = "FRONTEND_DB";
    private static final String HDFS_URL = "HDFS_URL";
    private static final String JOBTRACKER_URL = "JOBTRACKER_URL";
    private static final String PORT = "SERVER_SOCKET_PORT";
    private static final int DEFAULT_PORT = 22;

    private final PropertiesConfiguration properties;

    public Configuration(URL url) throws ConfigurationException {
        this.properties = new PropertiesConfiguration(url);
    }

    public String getDbName() {
        return this.properties.getString(DB_NAME);
    }

    public String getDbPassword() {
        return this.properties.getString(DB_PASSWORD);
    }

    public String getDbUser() {
        return this.properties.getString(DB_USER);
    }

    public String getFrontendDbUrl() {
        return this.properties.getString(FRONTEND_DB);
    }

    public URI getHdfsUrl() {
        return URI.create(this.properties.getString(HDFS_URL));
    }

    public String getJobTrackerUrl() {
        return this.properties.getString(JOBTRACKER_URL);
    }

    public int getPort() {
        return this.properties.getInt(PORT, DEFAULT_PORT);
    }
}
