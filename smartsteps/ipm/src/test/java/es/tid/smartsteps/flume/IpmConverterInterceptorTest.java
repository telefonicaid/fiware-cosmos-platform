package es.tid.smartsteps.flume;

import com.google.common.base.Charsets;
import es.tid.smartsteps.ipm.CrmRawToIpmConverter;
import es.tid.smartsteps.ipm.InetRawToIpmConverter;
import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.RawToIpmConverter;
import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.interceptor.Interceptor;
import org.junit.Before;
import org.junit.Test;

import java.util.LinkedList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

/**
 * Unit test for IPM converter interceptor
 *
 * @author apv
 */
public class IpmConverterInterceptorTest {

    private Interceptor crmInterceptor;
    private Interceptor inetInterceptor;

    private static final String validCrmEvent =
            "0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17";
    private static final String validInetEvent =
            "0|1|2|3|4|5|6|7|8|9|10|11|12|13";

    private Interceptor buildInterceptor(
            IpmConverterType converterType) {
        IpmConverterInterceptor.Builder interceptorBuilder =
                new IpmConverterInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(IpmConverterInterceptor.Constants.PROPERTY_CONVERTER,
                converterType.name());
        interceptorBuilder.configure(ctx);
        return interceptorBuilder.build();
    }

    private void interceptValidEvent(
            String inputLine,
            Interceptor interceptor,
            RawToIpmConverter converter) throws ParseException {
        Event inputEvent = EventBuilder.withBody(inputLine, Charsets.US_ASCII);
        Event outputEvent = interceptor.intercept(inputEvent);
        assertEquals(new String(outputEvent.getBody()),
                converter.convert(inputLine));
    }

    private void interceptInvalidEvent(
            Interceptor interceptor) throws ParseException {
        Event inputEvent = EventBuilder.withBody("Hello world!!",
                Charsets.US_ASCII);
        Event outputEvent = interceptor.intercept(inputEvent);
        assertNull(outputEvent);
    }

    private void interceptValidEventList(
            String inputLine,
            Interceptor interceptor,
            RawToIpmConverter converter) throws ParseException {
        List<Event> inputEvents = new LinkedList<Event>();
        for (int i = 0; i < 10; i++)
            inputEvents.add(EventBuilder.withBody(inputLine,
                    Charsets.US_ASCII));

        List<Event> outputEvents = interceptor.intercept(inputEvents);
        assertEquals(inputEvents.size(), outputEvents.size());

        String convertedEventBody = converter.convert(inputLine);
        for (int i = 0, l = inputEvents.size(); i < l; i++)
            assertEquals(new String(outputEvents.get(i).getBody()),
                    convertedEventBody);
    }

    @Before
    public void setUp() {
        crmInterceptor = buildInterceptor(IpmConverterType.CRM);
        inetInterceptor = buildInterceptor(IpmConverterType.INET);
    }

    @Test
    public void interceptValidCrmEvent() throws ParseException {
        interceptValidEvent(
                validCrmEvent, crmInterceptor, new CrmRawToIpmConverter());
    }

    @Test
    public void interceptInvalidCrmEvent() throws ParseException {
        interceptInvalidEvent(crmInterceptor);
    }

    @Test
    public void interceptValidCrmEventList() throws ParseException {
        interceptValidEventList(
                validCrmEvent, crmInterceptor, new CrmRawToIpmConverter());
    }

    @Test
    public void interceptValidInetEvent() throws ParseException {
        interceptValidEvent(
                validInetEvent, inetInterceptor, new InetRawToIpmConverter());
    }

    @Test
    public void interceptInvalidInetEvent() throws ParseException {
        interceptInvalidEvent(inetInterceptor);
    }

    @Test
    public void interceptValidInetEventList() throws ParseException {
        interceptValidEventList(
                validInetEvent, inetInterceptor, new InetRawToIpmConverter());
    }

}
