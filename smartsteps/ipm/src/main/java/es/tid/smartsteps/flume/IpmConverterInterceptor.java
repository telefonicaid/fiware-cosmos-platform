package es.tid.smartsteps.flume;

import java.util.ArrayList;
import java.util.List;

import com.google.common.base.Charsets;
import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.interceptor.Interceptor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.RawToIpmConverter;

import static es.tid.smartsteps.flume.IpmConverterInterceptor.Constants.PROPERTY_CONVERTER;

/**
 * A flume interceptor for converting incoming raw lines into IPM events.
 *
 * @author apv
 */
public class IpmConverterInterceptor implements Interceptor {
    private static final Logger logger =
            LoggerFactory.getLogger(IpmConverterInterceptor.class);

    private RawToIpmConverter converter;

    /**
     * Create a interceptor that uses given converter to map the events
     *
     * @param converter the converter used to map raw lines into IPM events
     */
    public IpmConverterInterceptor(RawToIpmConverter converter) {
        this.converter = converter;
    }

    @Override
    public void initialize() {
        // Nothing to do
    }

    @Override
    public Event intercept(Event event) {
        String body = new String(event.getBody());
        try {
            event.setBody(this.converter.convert(body).getBytes(
                    Charsets.US_ASCII));
            return event;
        } catch (ParseException e) {
            logger.warn("event discarded due to parsing error", e);
            return null;
        }
    }

    @Override
    public List<Event> intercept(List<Event> events) {
        List<Event> result = new ArrayList<Event>(events.size());
        for (Event inputEvent : events) {
            Event outputEvent = intercept(inputEvent);
            if (outputEvent != null) {
                result.add(outputEvent);
            }
        }
        return result;
    }

    @Override
    public void close() {
        // Nothing to do
    }

    public static class Builder implements Interceptor.Builder {
        private RawToIpmConverter converter = null;

        @Override
        public Interceptor build() {
            if (this.converter == null) {
                throw new RuntimeException("cannot build IPM converter " +
                        "interceptor: the builder has not been initialized yet");
            }
            return new IpmConverterInterceptor(this.converter);
        }

        @Override
        public void configure(Context context) throws ConfigurationException {
            String converterName = context.getString(PROPERTY_CONVERTER);
            if (converterName == null) {
                throw new ConfigurationException(String.format(
                        "missing '%s' property for IPM converter interceptor",
                        PROPERTY_CONVERTER));
            }
            Class<? extends RawToIpmConverter> converterClass = null;
            try {
                IpmConverterType converterType =
                        IpmConverterType.valueOf(converterName.toUpperCase());
                converterClass = converterType.getConverterClass();
            } catch (IllegalArgumentException ignorable) {
                // The converter name is unknown. Let's check whether it
                // is a valid class name implementing RawToIpmConverter
                try {
                    converterClass = (Class<? extends RawToIpmConverter>)
                            this.getClass().getClassLoader().loadClass(
                                    converterName);
                } catch (ClassNotFoundException e) {
                    throw new ConfigurationException(String.format(
                            "unknown value %s for '%s' property " +
                                    "of IPM converter interceptor",
                            converterName, PROPERTY_CONVERTER), e);
                } catch (ClassCastException e) {
                    throw new ConfigurationException(String.format(
                            "given value %s for '%s' property of IPM " +
                                    "converter interceptor is not valid: " +
                                    "not a %s instance",
                            converterName,
                            PROPERTY_CONVERTER,
                            RawToIpmConverter.class.getName()), e);
                }
            }
            try {
                this.converter = converterClass.newInstance();
            } catch (Exception e) {
                throw new ConfigurationException(String.format("cannot " +
                        "instantiate IPM converter from class %s",
                        converterClass.getName()), e);
            }
        }
    }

    public static class Constants {
        public static final String PROPERTY_CONVERTER = "converter";
    }
}
