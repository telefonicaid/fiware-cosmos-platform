package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.config.Config;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.parsing.CatchmentsParser;
import es.tid.smartsteps.footfalls.microgrids.parsing.LookupParser;

/**
 *
 * @author dmicol
 */
public class CatchmentsScalerReducerTest extends CatchmentsBasedTest {

    private ReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Catchments>> instance;
    private CatchmentsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> lookupValue;

    public CatchmentsScalerReducerTest() throws IOException {}

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Catchments>>(
                        new CatchmentsScalerReducer());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.parser = new CatchmentsParser();
        final Catchments parsedCatchments = this.parser.parse(this.catchments);
        this.countsValue = new TypedProtobufWritable<Message>(parsedCatchments);
        LookupParser lookupParser =
                new LookupParser(this.conf.get(Config.DELIMITER));
        this.lookupValue = new TypedProtobufWritable<Message>(
                lookupParser.parse("000012006440,polygon123,0.37"));
    }

    @Test
    public void testReduce() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Catchments>>> results =
                this.instance
                        .withInput(this.key,
                                   Arrays.asList(this.countsValue,
                                                 this.lookupValue))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Catchments>> result =
                results.get(0);
        assertEquals("polygon123", result.getFirst().toString());
        final Catchments outValue = result.getSecond().get();
        final Catchment catchment = outValue.getCatchments(0);
        assertEquals(0.37D, outValue.getCatchments(0).getTopCells(0).getCount(),
                     0.0D);
        assertEquals(0.74D, outValue.getCatchments(4).getTopCells(0).getCount(),
                     0.0D);
    }
}
