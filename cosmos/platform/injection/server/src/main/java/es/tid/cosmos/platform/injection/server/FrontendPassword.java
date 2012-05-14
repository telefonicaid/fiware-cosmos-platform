package es.tid.cosmos.platform.injection.server;

import java.sql.*;
import java.util.StringTokenizer;
import javax.security.auth.login.Configuration;
import javax.security.auth.login.LoginContext;
import javax.security.auth.login.LoginException;

import com.sun.security.auth.callback.TextCallbackHandler;
import com.sun.security.auth.login.ConfigFile;
import org.apache.commons.codec.digest.DigestUtils;
import org.apache.sshd.server.PasswordAuthenticator;
import org.apache.sshd.server.session.ServerSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * FrontendPassword
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since 08/05/12
 */
public class FrontendPassword implements PasswordAuthenticator {
    private String frontendDbUrl;
    private String dbName;
    private String dbUserName;
    private String dbPassword;
    private Connection connection;
    private final Logger LOG = LoggerFactory.getLogger(FrontendPassword.class);
    private static final String djangoSeparator = "$";

    @Override
    public boolean authenticate(String username,
                                String password, ServerSession session) {
        LOG.info(String.format("received %s as username, %s as password",
                username, password));
        try {
            this.connect(this.frontendDbUrl, this.dbName, this.dbUserName,
                    this.dbPassword);
            String sql = "SELECT password FROM auth_user WHERE username = ?";
            PreparedStatement preparedStatement =
                    this.connection.prepareStatement(sql);
            preparedStatement.setString(1, username);
            ResultSet resultSet = preparedStatement.executeQuery();
            String algorithm = "";
            String hash = "";
            String salt = "";
            while (resultSet.next()) {
                StringTokenizer algorithmSaltHash = new StringTokenizer(
                        resultSet.getString(1), djangoSeparator);
                algorithm = algorithmSaltHash.nextToken();
                salt = algorithmSaltHash.nextToken();
                hash = algorithmSaltHash.nextToken();
            }
            if (algorithm.equals("sha1")) {
                return hash.equals(DigestUtils.shaHex(salt + password));
            } else if (algorithm.equals("md5")) {
                return hash.equals(DigestUtils.md5Hex(salt + password));
            }
        } catch (SQLException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
        return false;
    }

    /**
     * connect to the platform frontend database with the configured
     * credentials
     *
     * @param url
     * @param dbName
     * @param userName
     * @param password
     */
    private void connect(String url, String dbName, String userName,
                         String password) {
        try {
            String driver = "org.sqlite.JDBC";
            if (url.contains("mysql")) {
                driver = "com.mysql.jdbc.Driver";
            }
            Class.forName(driver).newInstance();
            this.connection = DriverManager.getConnection(url+dbName, userName,
                    password);
        } catch (Exception e) {
            LOG.error(e.getMessage(), e);
        }
    }

    /**
     * set all instance variables required to connect to the platform frontend
     * database
     *
     * @param url
     * @param dbName
     * @param userName
     * @param password
     */
    public void setFrontendCredentials(String url, String dbName,
                                       String userName, String password) {
        this.frontendDbUrl = url;
        this.dbName = dbName;
        this.dbUserName = userName;
        this.dbPassword = password;
    }
}