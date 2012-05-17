package es.tid.cosmos.platform.injection.server;

import org.junit.*;

/**
 * InjectionServerTest
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since 15/05/12
 */
@Ignore
public class InjectionServerTest {
    private InjectionServer instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new InjectionServer(null);
    }

    @After
    public void tearDown() throws Exception {

    }

    @Test
    public void testSetupSftpServer() throws Exception {
        // TODO: implement testSetupSftpServer
        this.instance.setupSftpServer();
    }
}
