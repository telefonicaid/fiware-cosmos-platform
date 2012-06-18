package es.tid.cosmos.profile.export.text;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import static es.tid.cosmos.profile.data.CategoryCountUtil.create;
import es.tid.cosmos.profile.data.UserProfileUtil;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * @author sortega
 */
public class TextExporterReducerTest {
    private ReduceDriver<Text, ProtobufWritable<UserProfile>,
                         NullWritable, Text> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, ProtobufWritable<UserProfile>,
                NullWritable, Text>(new TextExporterReducer());
    }

    @Test
    public void testReduce() {
        String userId = "123";
        ProtobufWritable<UserProfile> user = UserProfileUtil.createAndWrap(
                userId, "2012/02/01", asList(create("SPORT", 10L),
                                             create("NEWS", 5)));
        this.driver.withInput(new Text(userId), asList(user))
                .withOutput(NullWritable.get(),
                            new Text("123\t2012/02/01\tSPORT\t10\tNEWS\t5"))
                .runTest();
    }
}
