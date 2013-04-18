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

package es.tid.cosmos.injection.server.persistence.db;

import java.net.ConnectException;
import java.security.PublicKey;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import sun.reflect.generics.reflectiveObjects.NotImplementedException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.injection.server.auth.UsersDao;

/**
 * Data Access Layer for retrieve Passwords of Users
 * Reads the password from MySQL database.
 *
 * @author ag453
 */
public class MysqlUsersDao implements UsersDao {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(MysqlUsersDao.class);
    private static final String SQL_SELECT_PASSWORD =
            "SELECT password FROM auth_user WHERE username = ?";

    private final PersistenceManager pm;

    public MysqlUsersDao(PersistenceManager pm) {
        this.pm = pm;
    }

    /**
     * Search user's password on a MySQL Database
     *
     * @param username Username on login process.
     * @return User's password codified as hashType$seed$passwordHash.
     * @throws ConnectException if cannot instantiate any connection.
     */
    @Override
    public String findPasswordByUsername(String username)
            throws ConnectException {
        ResultSet resultSet = null;
        PreparedStatement preparedStatement = null;
        Connection connection = null;
        try {
            connection = this.pm.getConnection();
            preparedStatement = connection.prepareStatement(
                    SQL_SELECT_PASSWORD);
            preparedStatement.setString(1, username);
            resultSet = preparedStatement.executeQuery();
            resultSet.next();
            return resultSet.getString(1);
        } catch (SQLException e) {
            LOGGER.error(e.getMessage(), e);
            return null;
        } finally {
            if (resultSet != null ) {
                try {
                    resultSet.close();
                } catch (SQLException e) {
                    LOGGER.error(
                            "unexpected exception while connecting to DB: " +
                            e.getMessage());
                }
            }
            if (preparedStatement != null) {
                try {
                    preparedStatement.close();
                } catch (SQLException e) {
                    LOGGER.warn("could not close a database statement", e);
                }
            }
            if (connection != null) {
                try {
                    connection.close();
                } catch (SQLException e) {
                    LOGGER.warn("could not close a database connection", e);
                }
            }
        }
    }

    /**
     * This method returns the Public Keys of user, but is not implemented
     * for MySQL persistence because is not needed for our application.
     *
     * @param username Username used on login process.
     * @return Iterable contains Public Keys accepted for this user.
     */
    @Override
    public Iterable<PublicKey> findPublicKeyByUsername(String username) {
        throw new NotImplementedException();
    }
}
