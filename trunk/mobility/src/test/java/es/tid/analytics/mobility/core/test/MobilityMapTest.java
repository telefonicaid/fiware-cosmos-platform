package es.tid.analytics.mobility.core.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.fail;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import es.tid.analytics.mobility.core.IndividualMobilityMap;
import es.tid.ps.mobility.data.BaseProtocol;
import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MobProtocol.GLEvent;

/**
 * User: masp20
 * Date: 06-feb-2012
 * Time: 12:49:38
 */
public class MobilityMapTest {

    private MapDriver<LongWritable, Text, LongWritable, ProtobufWritable<GLEvent>> driver;
    private static final String CDRS_LINE = "1376352479|2221435146|1376352479|0442221472843|2|LDN|20100104|17:21:07|22|118-TELEFONIA MOVIL|118-TELEFONIA MOVIL|??|??|11115006528440|NOROAMI";


    @Before
    public void setUp() {
        final Mapper<LongWritable, Text, LongWritable, ProtobufWritable<GLEvent>> mapper = new IndividualMobilityMap();
        this.driver = new MapDriver<LongWritable, Text, LongWritable, ProtobufWritable<GLEvent>>(mapper);
    }

//    @Test
//    public void testEmpty() {
//        List<Pair<LongWritable, GLEvent>> out = null;
//
//        try {
//            out = this.driver.withInput(new LongWritable(0), new Text("")).run();
//        } catch (IOException ioe) {
//            fail();
//        }
//
//        final List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
//
//        assertListEquals(expected, out);
//    }

    @Test @Ignore
    public void testCDRLine() {
        //TODO : don't work properly without cellCatalogue
        List<Pair<LongWritable, ProtobufWritable<GLEvent>>> out = null;

        try {
            out = this.driver.withInput(new LongWritable(0), new Text(CDRS_LINE)).run();
        } catch (IOException ioe) {
            fail();
        }

        final LongWritable nodeId = new LongWritable(2221435146L);
        final GLEvent glEvent = createEvent();

        final List<Pair<LongWritable, GLEvent>> expected = new ArrayList<Pair<LongWritable, GLEvent>>();
        expected.add(new Pair<LongWritable, GLEvent>(nodeId, glEvent));

        assertListEquals(expected, out);
    }

    private GLEvent createEvent() {
        final GLEvent.Builder glEvent = GLEvent.newBuilder();
        final Date.Builder testDate = Date.newBuilder();
        final Time.Builder testTime = Time.newBuilder();
        
        testDate.setDay(4);
        testDate.setMonth(1);
        testDate.setYear(2010);
        testDate.setWeekday(2);
        testTime.setHour(17);
        testTime.setMinute(21);
        testTime.setSeconds(7);

        glEvent.setUserId(2221435146L);
        glEvent.setPlaceId(213094);
        glEvent.setDate(testDate);
        glEvent.setTime(testTime);
        return glEvent.build();
    }

}
