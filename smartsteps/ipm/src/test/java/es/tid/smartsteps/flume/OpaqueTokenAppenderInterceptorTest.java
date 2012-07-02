package es.tid.smartsteps.flume;

import java.nio.charset.Charset;
import java.util.UUID;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.interceptor.Interceptor;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.platform.injection.flume.OpaqueTokenConstants;

import static junit.framework.Assert.*;

/**
 * Unit test for opaque token appender interceptor.
 *
 * @author apv
 */
public class OpaqueTokenAppenderInterceptorTest {

    private Interceptor interceptor;
    private UUID opaqueToken;

    @Before
    public void setup() {
        this.opaqueToken = UUID.randomUUID();

        OpaqueTokenAppenderInterceptor.Builder builder =
                new OpaqueTokenAppenderInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(OpaqueTokenAppenderInterceptor.Builder.PROPERTY_OPAQUE_TOKEN,
                this.opaqueToken.toString());
        builder.configure(ctx);
        this.interceptor = builder.build();
    }

    @Test
    public void testInterceptEvent() {
        Event inputEvent = EventBuilder.withBody("Hello world!",
                Charset.forName("UTF-8"));
        Event outputEvent = this.interceptor.intercept(inputEvent);
        assertNotNull(outputEvent);
        assertEquals(outputEvent.getHeaders().get(
                OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN),
                this.opaqueToken.toString());
    }

    @Test
    public void testInvalidEvent() {
        Event inputEvent = EventBuilder.withBody("Hello world!",
                Charset.forName("UTF-8"));
        inputEvent.getHeaders().put(
                OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN,
                this.opaqueToken.toString());
        Event outputEvent = this.interceptor.intercept(inputEvent);
        assertNull(outputEvent);
    }

    @Test(expected = ConfigurationException.class)
    public void testConfigureInterceptorWithMissingToken() {
        OpaqueTokenAppenderInterceptor.Builder builder =
                new OpaqueTokenAppenderInterceptor.Builder();
        builder.configure(new Context());
    }

    @Test(expected = ConfigurationException.class)
    public void testConfigureInterceptorWithInvalidToken() {
        OpaqueTokenAppenderInterceptor.Builder builder =
                new OpaqueTokenAppenderInterceptor.Builder();
        Context ctx = new Context();
        ctx.put(OpaqueTokenAppenderInterceptor.Builder.PROPERTY_OPAQUE_TOKEN,
                "123456789");
        builder.configure(ctx);
    }

}
