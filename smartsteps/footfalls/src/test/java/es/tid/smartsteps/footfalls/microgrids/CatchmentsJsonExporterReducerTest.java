package es.tid.smartsteps.footfalls.microgrids;

import static java.util.Arrays.asList;

import net.sf.json.JSONObject;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.parsing.CatchmentsParser;

/**
 *
 * @author sortega
 */
public class CatchmentsJsonExporterReducerTest extends CatchmentsBasedTest {

    private CatchmentsParser parser;
    private ReduceDriver<
            Text, TypedProtobufWritable<Catchments>,
            NullWritable, Text> instance;

    @Before
    public void setUp() {
        this.parser = mock(CatchmentsParser.class);
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Catchments>,
                NullWritable, Text>(new CatchmentsJsonExporterReducer(this.parser));
    }

    @Test
    public void shouldSerializeToJson() throws Exception {
        Catchments inputValue = Catchments.newBuilder()
                .setId("cellId")
                .setDate(SAMPLE_DATE)
                .setLatitude(1d)
                .setLongitude(-1d)
                .build();

        JSONObject expectedOutput = new JSONObject();
        expectedOutput.put("canned", "output");

        when(parser.toJSON(inputValue, false))
                .thenReturn(expectedOutput);

        this.instance
                .withInput(new Text("key"), asList(
                    new TypedProtobufWritable<Catchments>(inputValue)))
                .withOutput(NullWritable.get(),
                            new Text(expectedOutput.toString()))
                .runTest();
    }
}