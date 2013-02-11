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

package es.tid.cosmos.platform.injection.server.persistence.db;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

import es.tid.cosmos.base.util.Logger;

/**
 * Creates and manages the connection to database using JDBC API
 *
 * @author ag453
 */
public class JDBCPersistenceManager implements PersistenceManager {

    private static final String MYSQL_DRIVER = "com.mysql.jdbc.Driver";
    private static final String SQLITE_DRIVER = "org.sqlite.JDBC";
    private static final String SQLITE_CONN_STRING = "jdbc:sqlite://";
    private static final String MYSQL_CONN_STRING = "jdbc:sqlite://";
    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(JDBCPersistenceManager.class);

    private final String url;
    private final String dbName;
    private final String userName;
    private final String password;

    public JDBCPersistenceManager(
            String url, String dbName, String userName, String password) {
        this.url = url;
        this.dbName = dbName;
        this.userName = userName;
        this.password = password;
    }

    /**
     * Create a JDBC Connection with configured credentials
     */
    public Connection getConnection() {
        try {
            Class.forName(this.getDriver()).newInstance();
            return DriverManager.getConnection(
                    this.url + this.dbName, this.userName, this.password);
        } catch (Exception e) {
            LOGGER.error("unexpected exception while connecting to DB" +
                e.getMessage());
            return null;
        }
    }

    private String getDriver() throws SQLException {
        if (this.url.startsWith(SQLITE_CONN_STRING)) {
            return SQLITE_DRIVER;
        } else if (this.url.startsWith(MYSQL_CONN_STRING)) {
            return MYSQL_DRIVER;
        } else {
            throw new SQLException(
                "Cannot determine any driver from connection " +
                "chain '%s'", this.url);
        }
    }
}
