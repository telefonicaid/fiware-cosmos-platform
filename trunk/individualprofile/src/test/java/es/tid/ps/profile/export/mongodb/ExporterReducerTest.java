package es.tid.ps.profile.export.mongodb;

import com.mongodb.hadoop.io.BSONWritable;
import static java.util.Arrays.*;
import java.util.List;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.bson.BSONObject;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.ps.profile.userprofile.CategoryCount;
import es.tid.ps.profile.userprofile.UserProfile;

/**
 *
 * @author sortega
 */
public class ExporterReducerTest {
    private ReduceDriver<Text, UserProfile, MongoProperty, BSONWritable> driver;
    private Text userId;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, UserProfile, 
                MongoProperty, BSONWritable>(new ExporterReducer());
        this.userId = new Text("USER00123");
    }

    @Test
    public void testReduce() throws Exception {
        UserProfile p1 = new UserProfile();
        p1.setUserId(userId.toString());
        p1.setDate("2012/02/01");
        p1.add(new CategoryCount("Sport", 5L));
        p1.add(new CategoryCount("Lifestyle", 10L));
        
        List<Pair<MongoProperty, BSONWritable>> results = 
                driver.withInput(userId, asList(p1)).run();
        
        assertEquals(1, results.size());
        Pair<MongoProperty, BSONWritable> result = results.get(0);
        assertEquals("id", result.getFirst().getProperty());
        assertEquals("USER00123", result.getFirst().getValue());
        assertEquals(5L, ((BSONObject)result.getSecond().get("categories"))
                .get("Sport"));
    }
}
