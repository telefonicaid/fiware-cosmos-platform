package es.tid.bdp.profile.userprofile;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class UserProfileJobTest {
    private UserProfileJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new UserProfileJob(new Configuration());
    }
    
    @Test
    public void testConfigure() throws IOException {
        this.instance.configure(new Path("/opt/test1"), new Path("/opt/test2"));
    }
}
