package es.tid.cosmos.platform.injection.server;

import java.io.File;

import org.junit.*;
import static junit.framework.Assert.assertTrue;

/**
 * InjectionServerTest
 *
 * @author logc
 * @since 15/05/12
 */
public class InjectionServerTest {
    private InjectionServer instance;

    @Before
    public void setUp() throws Exception {
        Configuration configuration = new Configuration(
                InjectionServerMain.class
                        .getResource("/injection_server.dev.properties"));
        this.instance = new InjectionServer(configuration);
    }

    @After
    public void tearDown() throws Exception {

    }

    @Test
    public void testSetupSftpServer() throws Exception {
        assertTrue(this.instance instanceof InjectionServer);
        // No exceptions thrown
        this.instance.setupSftpServer();
    }
}
