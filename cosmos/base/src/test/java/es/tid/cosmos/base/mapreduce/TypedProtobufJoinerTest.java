package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.SampleProtocol.MessageA;
import es.tid.cosmos.base.data.generated.SampleProtocol.MessageB;
import es.tid.cosmos.base.data.generated.SampleProtocol.MessageC;

/**
 * @author sortega
 */
public class TypedProtobufJoinerTest {

    private ReduceDriver<Text, TypedProtobufWritable<Message>,
            Text, LongWritable> joinDriver;
    private Text key;

    private static class ABJoiner extends TypedProtobufJoiner<
            Text, MessageA, MessageB,
            Text, LongWritable> {

        public ABJoiner() {
            super(MessageA.class, MessageB.class);
        }

        @Override
        protected void join(Text key, List<? extends MessageA> leftList,
                            List<? extends MessageB> rightList, Context context)
                throws IOException, InterruptedException {
            context.write(key, new LongWritable(leftList.size() * rightList.size()));
        }
    }

    @Before
    public void setUp() throws Exception {
        this.joinDriver = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, LongWritable>(new ABJoiner());
        this.key = new Text("key");
    }

    @Test
    public void shouldUnwrapAndSplitInputValues() throws Exception {
        this.joinDriver
                .withInput(this.key, TypedProtobufWritable.<Message>asList(
                        MessageA.newBuilder().setId("1").build(),
                        MessageA.newBuilder().setId("2").build(),
                        MessageA.newBuilder().setId("2").build(),
                        MessageB.newBuilder().setId("a").build(),
                        MessageB.newBuilder().setId("b").build()))
                .withOutput(this.key, new LongWritable(6))
                .runTest();
    }

    @Test(expected = IllegalStateException.class)
    public void shouldFailOnUnexpectedMessages() throws Exception {
        this.joinDriver
                .withInput(this.key, TypedProtobufWritable.<Message>asList(
                        MessageA.newBuilder().setId("1").build(),
                        MessageB.newBuilder().setId("a").build(),
                        MessageC.newBuilder().setId("omega").build()))
                .run();
    }
}
