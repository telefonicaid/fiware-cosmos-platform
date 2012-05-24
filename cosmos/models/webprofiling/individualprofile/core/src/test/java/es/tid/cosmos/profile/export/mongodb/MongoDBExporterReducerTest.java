package es.tid.cosmos.profile.export.mongodb;

import static java.util.Arrays.asList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.bson.BSONObject;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import static es.tid.cosmos.profile.data.CategoryCountUtil.create;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Use case for ExporterReducer
 *
 * @author sortega
 */
public class MongoDBExporterReducerTest {
    private ReduceDriver<Text, ProtobufWritable<UserProfile>, MongoProperty,
            BSONWritable> driver;
    private Text userId;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, ProtobufWritable<UserProfile>,
                MongoProperty, BSONWritable>(new MongoDBExporterReducer());
        this.userId = new Text("USER00123");
    }

    @Test
    public void testReduce() throws Exception {
        UserProfile p1 = UserProfile
                .newBuilder()
                .setUserId(userId.toString())
                .setDate("2012/02/01")
                .addCounts(create("Sport", 5L))
                .addCounts(create("Lifestyle", 10L))
                .build();

        ProtobufWritable<UserProfile> wrappedP1 =
                new ProtobufWritable<UserProfile>();
        wrappedP1.setConverter(UserProfile.class);
        wrappedP1.set(p1);

        List<Pair<MongoProperty, BSONWritable>> results =
                driver.withInput(userId, asList(wrappedP1)).run();

        assertEquals(1, results.size());
        Pair<MongoProperty, BSONWritable> result = results.get(0);
        assertEquals("id", result.getFirst().getProperty());
        assertEquals("USER00123", result.getFirst().getValue());
        assertEquals(5L, ((BSONObject)result.getSecond().get("categories"))
                .get("Sport"));
    }
}
