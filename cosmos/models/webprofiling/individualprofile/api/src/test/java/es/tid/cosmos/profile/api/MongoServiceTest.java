package es.tid.cosmos.profile.api;

import org.junit.After;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

/**
 * @author sortega
 */
public class MongoServiceTest {
    private MongoService instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new MongoService();
        this.instance.setHostname("localhost");
        this.instance.setDatabaseName("db1");
        this.instance.init();
    }

    @After
    public void tearDown() {
        this.instance.destroy();
    }

    @Test
    public void testGetDb() {
        assertNotNull(this.instance.getDb());
    }

    @Test
    public void testGetMongo() {
        assertNotNull(this.instance.getMongo());
    }
}
