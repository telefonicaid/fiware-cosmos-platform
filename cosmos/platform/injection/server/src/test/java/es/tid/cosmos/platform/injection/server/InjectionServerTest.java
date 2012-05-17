package es.tid.cosmos.platform.injection.server;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * InjectionServerTest
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since 15/05/12
 */
public class InjectionServerTest {
    private InjectionServer instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new InjectionServer();
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
