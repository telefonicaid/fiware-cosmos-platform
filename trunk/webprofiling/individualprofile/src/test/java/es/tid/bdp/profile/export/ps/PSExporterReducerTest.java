package es.tid.bdp.profile.export.ps;

import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import static es.tid.bdp.profile.data.CategoryCountUtil.create;
import es.tid.bdp.profile.data.ProfileProtocol.UserProfile;

/**
 * Use case for ExporterReducer
 *
 * @author dmicol
 */
public class PSExporterReducerTest {
    private ReduceDriver<Text, ProtobufWritable<UserProfile>, NullWritable,
                         Text> driver;
    private Text userId;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, ProtobufWritable<UserProfile>,
                NullWritable, Text>(new PSExporterReducer());
        this.userId = new Text("USER00123");
    }

    @Test
    public void testReduce() throws Exception {
        UserProfile p1 = UserProfile
                .newBuilder()
                .setUserId(this.userId.toString())
                .setDate("2012/02/01")
                .addCounts(create("Sport", 5L))
                .addCounts(create("Lifestyle", 10L))
                .build();
        UserProfile p2 = UserProfile
                .newBuilder()
                .setUserId(this.userId.toString())
                .setDate("2012/02/02")
                .addCounts(create("News", 3L))
                .addCounts(create("Shopping", 16L))
                .build();

        ProtobufWritable<UserProfile> wrappedP1 =
                new ProtobufWritable<UserProfile>();
        wrappedP1.setConverter(UserProfile.class);
        wrappedP1.set(p1);
        ProtobufWritable<UserProfile> wrappedP2 =
                new ProtobufWritable<UserProfile>();
        wrappedP2.setConverter(UserProfile.class);
        wrappedP2.set(p2);

        List<Pair<NullWritable, Text>> results =
                this.driver.withInput(this.userId,
                                      asList(wrappedP1,wrappedP2)).run();
        assertEquals(4, results.size());
        assertEquals("User|Sport|Lifestyle",
                     results.get(0).getSecond().toString());
        assertEquals("USER00123_2012/02/01|5|10",
                     results.get(1).getSecond().toString());
        assertEquals("USER00123_2012/02/02|3|16",
                     results.get(2).getSecond().toString());
        assertEquals("2",
                     results.get(3).getSecond().toString());
    }
}
