package es.tid.cosmos.platform.injection.server;

import java.sql.*;
import java.util.StringTokenizer;
import javax.security.auth.login.Configuration;
import javax.security.auth.login.LoginContext;
import javax.security.auth.login.LoginException;

import com.sun.security.auth.callback.TextCallbackHandler;
import com.sun.security.auth.login.ConfigFile;
import org.apache.commons.codec.digest.DigestUtils;
import org.apache.mina.proxy.handlers.http.digest.DigestUtilities;
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
    private final Logger LOG = LoggerFactory.getLogger(FrontendPassword.class);
    private final String djangoSeparator = "$";
    private Connection connection;

    @Override
    public boolean authenticate(String username,
                                String password, ServerSession session) {
        LOG.info(String.format("received %s as username, %s as password",
                username, password));
        try {
            connect();
            String sql = "SELECT password FROM auth_user WHERE username = ?";
            PreparedStatement preparedStatement =
                    this.connection.prepareStatement(sql);
            preparedStatement.setString(1, username);
            ResultSet resultSet = preparedStatement.executeQuery();
            while (resultSet.next()) {
                StringTokenizer algorithmSaltHash = new StringTokenizer(
                        resultSet.getString(1), this.djangoSeparator);
                String algorithm = algorithmSaltHash.nextToken();
                String salt = algorithmSaltHash.nextToken();
                String hash = algorithmSaltHash.nextToken();
                if (algorithm.equals("sha1")) {
                    if (hash.equals(DigestUtils.shaHex(salt + password))) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            // TODO: set config file path
            System.setProperty("java.security.auth.login.config",
                    "login.config");
            ConfigFile configFile = new ConfigFile();
            Configuration.setConfiguration(configFile);
            LoginContext loginContext = new LoginContext("LoginUnix",
                    new TextCallbackHandler());
            loginContext.login();
        } catch (LoginException e) {
            LOG.error(e.getMessage(), e);
            return false;
        } catch (SQLException e) {
            LOG.error(e.getMessage(), e);
            return false;
        }
        return true;
    }

    private void connect() {
//        String url = "jdbc:mysql://localhost:3306/";
        String url = "jdbc:sqlite:/tmp/cosmos.db";
        String dbName = "";
        String driver = "org.sqlite.JDBC";
        String userName = "root";
        String password = "root";
        try {
            Class.forName(driver).newInstance();
            this.connection = DriverManager.getConnection(url+dbName, userName,
                    password);
        } catch (Exception e) {
            LOG.error(e.getMessage(), e);
        }
    }
}
