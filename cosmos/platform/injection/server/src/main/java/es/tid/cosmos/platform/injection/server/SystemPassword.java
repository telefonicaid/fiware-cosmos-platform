package es.tid.cosmos.platform.injection.server;

import javax.security.auth.Subject;
import javax.security.auth.login.AppConfigurationEntry;
import javax.security.auth.login.Configuration;
import javax.security.auth.login.LoginContext;
import javax.security.auth.login.LoginException;

import com.sun.security.auth.UnixPrincipal;
import com.sun.security.auth.callback.TextCallbackHandler;
import com.sun.security.auth.login.ConfigFile;
import org.apache.sshd.server.PasswordAuthenticator;
import org.apache.sshd.server.session.ServerSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * SystemPassword
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since 08/05/12
 */
public class SystemPassword implements PasswordAuthenticator {
    private final Logger LOG = LoggerFactory.getLogger(SystemPassword.class);

    @Override
    public boolean authenticate(String username, String password,
                                ServerSession session) {
        LOG.info(String.format("received %s as username, %s as password",
                username, password));
        try {
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
        }
        return true;
    }
}
