package es.tid.smartsteps.flume;

import java.nio.charset.Charset;
import java.util.LinkedList;
import java.util.List;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.interceptor.Interceptor;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.ipm.CrmRawToIpmConverter;
import es.tid.smartsteps.ipm.InetRawToIpmConverter;
import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.RawToIpmConverter;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
/**
 * Unit test for IPM converter interceptor
 *
 * @author apv
 */
public class IpmConverterInterceptorTest {
    private static final String validCrmEvent =
            "0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17";
    private static final String validInetEvent =
            "0|1|2|3|4|5|6|7|8|9|10|11|12|13";
    private static final String eventDelimiter = "|";
    private static final Charset eventCharset = Charset.forName("UTF-8");

    private Interceptor crmInterceptor;
    private Interceptor inetInterceptor;

    /**
     * Take IPM converter class. This class provides a fake implementation
     * of IPM converter for testing purposes.
     */
    public static class FakeIpmConverter implements RawToIpmConverter {
        @Override
        public String convert(String line) throws ParseException {
            return line;
        }

        public static final class Builder implements RawToIpmConverter.Builder {
            @Override
            public RawToIpmConverter newConverter(String delimiter,
                                                  Charset charset) {
                return new FakeIpmConverter();
            }
        }
    }

    private Interceptor buildInterceptor(IpmConverterType converterType) {
        IpmConverterInterceptor.Builder interceptorBuilder =
                new IpmConverterInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(IpmConverterInterceptor.PROPERTY_CONVERTER,
                converterType.name());
        ctx.put(IpmConverterInterceptor.PROPERTY_DELIMITER, "|");
        ctx.put(IpmConverterInterceptor.PROPERTY_CHARSET, "UTF-8");
        interceptorBuilder.configure(ctx);
        return interceptorBuilder.build();
    }

    private void interceptValidEvent(String inputLine,
            Interceptor interceptor,
            RawToIpmConverter converter) throws ParseException {
        Event inputEvent = EventBuilder.withBody(inputLine,
                Charset.forName("UTF-8"));
        Event outputEvent = interceptor.intercept(inputEvent);
        assertEquals(new String(outputEvent.getBody()), converter.convert(inputLine));
    }

    private void interceptInvalidEvent(
            Interceptor interceptor) throws ParseException {
        Event inputEvent = EventBuilder.withBody("Hello world!!",
                Charset.forName("UTF-8"));
        Event outputEvent = interceptor.intercept(inputEvent);
        assertNull(outputEvent);
    }

    private void interceptValidEventList(String inputLine,
            Interceptor interceptor,
            RawToIpmConverter converter) throws ParseException {
        List<Event> inputEvents = new LinkedList<Event>();
        for (int i = 0; i < 10; i++) {
            inputEvents.add(EventBuilder.withBody(inputLine,
                    Charset.forName("UTF-8")));
        }

        List<Event> outputEvents = interceptor.intercept(inputEvents);
        assertEquals(inputEvents.size(), outputEvents.size());

        String convertedEventBody = converter.convert(inputLine);
        for (int i = 0, l = inputEvents.size(); i < l; i++)
            assertEquals(new String(outputEvents.get(i).getBody()),
                    convertedEventBody);
    }

    @Before
    public void setUp() {
        this.crmInterceptor = buildInterceptor(IpmConverterType.CRM);
        this.inetInterceptor = buildInterceptor(IpmConverterType.INET);
    }

    @Test(expected = ConfigurationException.class)
    public void testConfigureInterceptorFromInvalidConverter() {
        IpmConverterInterceptor.Builder interceptorBuilder =
                new IpmConverterInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(IpmConverterInterceptor.PROPERTY_CONVERTER,
                "foobar");
        ctx.put(IpmConverterInterceptor.PROPERTY_DELIMITER, "|");
        ctx.put(IpmConverterInterceptor.PROPERTY_CHARSET, "UTF-8");
        interceptorBuilder.configure(ctx);
    }

    @Test
    public void testConfigureInterceptorFromCustomConverter() {
        IpmConverterInterceptor.Builder interceptorBuilder =
                new IpmConverterInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(IpmConverterInterceptor.PROPERTY_CONVERTER,
                FakeIpmConverter.class.getName());
        ctx.put(IpmConverterInterceptor.PROPERTY_DELIMITER, "|");
        ctx.put(IpmConverterInterceptor.PROPERTY_CHARSET, "UTF-8");
        interceptorBuilder.configure(ctx);
    }

    @Test
    public void testInterceptValidCrmEvent() throws ParseException {
        this.interceptValidEvent(
                this.validCrmEvent, this.crmInterceptor,
                new CrmRawToIpmConverter(eventDelimiter, eventCharset));
    }

    @Test
    public void testInterceptInvalidCrmEvent() throws ParseException {
        this.interceptInvalidEvent(this.crmInterceptor);
        interceptInvalidEvent(this.crmInterceptor);
    }

    @Test
    public void testInterceptValidCrmEventList() throws ParseException {
        this.interceptValidEventList(
                this.validCrmEvent, this.crmInterceptor,
                new CrmRawToIpmConverter(eventDelimiter, eventCharset));
    }

    @Test
    public void testInterceptValidInetEvent() throws ParseException {
        this.interceptValidEvent(
                this.validInetEvent, this.inetInterceptor,
                new InetRawToIpmConverter(eventDelimiter, eventCharset));
    }

    @Test
    public void testInterceptInvalidInetEvent() throws ParseException {
        this.interceptInvalidEvent(this.inetInterceptor);
    }

    @Test
    public void testInterceptValidInetEventList() throws ParseException {
        this.interceptValidEventList(
                this.validInetEvent, this.inetInterceptor,
                new InetRawToIpmConverter(eventDelimiter, eventCharset));
    }
}
