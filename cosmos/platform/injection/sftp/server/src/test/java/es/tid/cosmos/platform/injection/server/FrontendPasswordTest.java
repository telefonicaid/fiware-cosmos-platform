package es.tid.cosmos.platform.injection.server;

import java.io.File;
import java.sql.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;

/**
 * FrontendPasswordTest
 *
 * @author logc
 * @since 15/05/12
 */
public class FrontendPasswordTest {
    private static final Logger LOG = LoggerFactory.getLogger(
            FrontendPasswordTest.class);

    private FrontendPassword instance;
    private String fileName;
    private String frontendDbUrl;
    private Connection connection;

    @Before
    public void setUp() throws Exception {
        this.instance = new FrontendPassword();
        this.fileName = "test.db";
        this.frontendDbUrl = "jdbc:sqlite:" + fileName;
        Class.forName("org.sqlite.JDBC").newInstance();
        this.connection = DriverManager.getConnection(this.frontendDbUrl);
        Statement stat = this.connection.createStatement();
        stat.executeUpdate("DROP TABLE IF EXISTS auth_user");
        //  As dumped from frontend temporary database on 2012-05-15 with the
        //  following commands:
        //  bin/django syncdb; sqlite3 /tmp/cosmos.db; > .schema auth_user

        //  Unused fields (e.g. first_name) were ignored
        String createStatement = "CREATE TABLE \"auth_user\" (\n" +
                "    \"username\" varchar(30) NOT NULL UNIQUE,\n" +
                "    \"password\" varchar(128) NOT NULL\n" +
                ");";
        stat.executeUpdate(createStatement);
        // as dumped from frontend temporary database on 2012-05-15 with the
        // following commands:
        // > SELECT username, password FROM auth_user;
        String usernameColContent1 = "test";
        String passwordColContent1 =
                "sha1$a49dc$b234ed692454a6164983c3fae6d8b4ca0f69b219";
        String usernameColContent2 = "testMd5";
        String passwordColContent2 =
                "md5$a49dc$de03e38fd7240af348801f09ab2ed616";
        String usernameColContent3 = "testSenselessContent";
        String passwordColContent3 =
                "colt45$a49dc$de03e38fd7240af348801f09ab2ed616";

        this.insertIntoTestDb(usernameColContent1, passwordColContent1);
        this.insertIntoTestDb(usernameColContent2, passwordColContent2);
        this.insertIntoTestDb(usernameColContent3, passwordColContent3);
    }

    private void insertIntoTestDb(String usernameColContent,
                String passwordColContent) throws SQLException {
        String sql = "INSERT INTO auth_user(username, password) " +
                "VALUES (?, ?)";
        PreparedStatement insertion = this.connection.prepareStatement(sql);
        insertion.setString(1, usernameColContent);
        insertion.setString(2, passwordColContent);
        insertion.execute();
    }

    @After
    public void tearDown() throws Exception {
        this.deleteDbFile();
    }

    private void deleteDbFile() {
        boolean deleted = false;
        File f = new File(this.fileName);
        if (f.exists() && f.canWrite() && !f.isDirectory()) {
            deleted = f.delete();
        }
        if (!deleted) {
            LOG.error("test DB at " + this.fileName + "could not be deleted");
        }
    }

    /**
     * The PasswordAuthenticator interface requires us to catch any exceptions
     * thrown by the underlying authentication, including SQL databases not
     * found, etc ...
     *
     * Here we show that the exception is caught and we do not allow
     * authentication if the credentials are correct but the database is missing
     *
     * @throws Exception
     */
    @Test
    public void testWithoutDb() throws Exception {
        this.instance.setFrontendCredentials(this.frontendDbUrl, "", "", "");
        this.deleteDbFile();
        assertFalse(this.instance.authenticate("who_cares", "whatever",
                null));
    }

    @Test
    public void testAuthenticate() throws Exception {
        this.instance.setFrontendCredentials(this.frontendDbUrl, "", "", "");
        assertFalse(this.instance.authenticate("test", "fake_password", null));
        assertTrue(this.instance.authenticate("test", "test", null));
        assertFalse(this.instance.authenticate("testMd5", "very_fake", null));
        assertTrue(this.instance.authenticate("testMd5", "test", null));
    }

    /**
     * Show that if the content found in the database does not make sense, we
     * do not allow authentication. Note that one user does not exist and the
     * other has 'colt45' stored as its password hashing algorithm.
     *
     * @throws Exception
     */
    @Test
    public void testDefaultAuthenticationIsFalse() throws Exception {
        this.instance.setFrontendCredentials(this.frontendDbUrl, "", "", "");
        assertFalse(this.instance.authenticate("anonymous",
                "we_are_legion", null));
        assertFalse(this.instance.authenticate("testSenselessContent",
                "whatever", null));
    }

    /**
     * Show that if the database URL is wrongly configured, we catch the
     * exception and do not allow to authenticate.
     * @throws Exception
     */
    @Test
    public void testAuthenticationWhenDbConfigWrong() throws Exception {
        this.instance.setFrontendCredentials("jdbc:superdb:cosmos.db", "", "",
                "");
        assertFalse(this.instance.authenticate("test", "test", null));
    }
}
