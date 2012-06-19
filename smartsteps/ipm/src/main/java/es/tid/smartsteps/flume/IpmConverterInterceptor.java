package es.tid.smartsteps.flume;

import java.nio.charset.Charset;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;
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

/**
 * A flume interceptor for converting incoming raw lines into IPM events.
 *
 * @author apv
 */
public class IpmConverterInterceptor implements Interceptor {
    private static final Logger logger =
            LoggerFactory.getLogger(IpmConverterInterceptor.class);

    public static final String PROPERTY_CONVERTER = "converter";
    public static final String PROPERTY_DELIMITER = "delimiter";
    public static final String PROPERTY_CHARSET   = "charset";

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

        private static RawToIpmConverter.Builder getBuilderFromClass(
                String className) throws ConfigurationException {
            ClassLoader cl = Builder.class.getClassLoader();
            Class<? extends RawToIpmConverter> converterClass;
            Class<? extends RawToIpmConverter.Builder> factoryClass;
            try {
                converterClass = (Class<? extends RawToIpmConverter>)
                        cl.loadClass(className);

            } catch (ClassNotFoundException e) {
                throw new ConfigurationException(String.format(
                        "invalid value %s for '%s' property; unknown class " +
                                "name",
                        className, PROPERTY_CONVERTER), e);
            } catch (ClassCastException e) {
                throw new ConfigurationException(String.format(
                        "invalid value %s for '%s' property; " +
                                "given class is not an instance of %s ",
                        className, PROPERTY_CONVERTER,
                        RawToIpmConverter.class.getName()), e);
            }
            try {
                factoryClass = (Class<? extends RawToIpmConverter.Builder>)
                        cl.loadClass(className + "$Builder");
            } catch (ClassNotFoundException e) {
                throw new ConfigurationException(String.format(
                        "invalid value %s for '%s' property; missing " +
                                "inner class Builder",
                        className, PROPERTY_CONVERTER), e);
            } catch (ClassCastException e) {
                throw new ConfigurationException(String.format(
                        "invalid value %s for '%s' property; " +
                                "given inner class Builder is not an instance" +
                                " of %s",
                        className, PROPERTY_CONVERTER,
                        RawToIpmConverter.class.getName()), e);
            }
            try {
                return factoryClass.newInstance();
            } catch (Exception e) {
                throw new ConfigurationException(String.format(
                        "invalid value %s for '%s' property; inner class " +
                                "Builder for given class cannot be " +
                                "instantiated",
                        className, PROPERTY_CONVERTER), e);
            }
        }

        @Override
        public Interceptor build() {
            if (this.converter == null) {
                throw new RuntimeException("cannot build IPM converter " +
                        "interceptor: the builder has not been initialized yet");
            }
            return new IpmConverterInterceptor(this.converter);
        }

        private static void checkPropertiesOrThrow(Context ctx)
                throws ConfigurationException {
            for (String property : new String[] { PROPERTY_CONVERTER,
                    PROPERTY_DELIMITER, PROPERTY_CHARSET }) {
                if (ctx.getString(property) == null) {
                    throw new ConfigurationException(String.format(
                            "missing '%s' property for IPM converter interceptor",
                            property));
                }
            }
        }

        @Override
        public void configure(Context context) throws ConfigurationException {
            checkPropertiesOrThrow(context);

            String converterName = context.getString(PROPERTY_CONVERTER);
            String delimiter = context.getString(PROPERTY_DELIMITER);
            String charset = context.getString(PROPERTY_CHARSET);

            RawToIpmConverter.Builder converterBuilder;
            try {
                IpmConverterType converterType =
                        IpmConverterType.valueOf(converterName.toUpperCase());
                converterBuilder = converterType.getConverterBuilder();
            } catch (IllegalArgumentException ignorable) {
                // The converter name is unknown. Let's check whether it
                // is a valid class name implementing RawToIpmConverter
                converterBuilder = getBuilderFromClass(converterName);
            }
            try {
                this.converter = converterBuilder.newConverter(delimiter,
                        Charset.forName(charset));
            } catch (IllegalCharsetNameException e) {
                throw new ConfigurationException(String.format("invalid value" +
                        " %s for property %s: unknown charset name",
                        charset, PROPERTY_CHARSET), e);
            } catch (UnsupportedCharsetException e) {
                throw new ConfigurationException(String.format("invalid value" +
                        " %s for property %s: charset is not supported",
                        charset, PROPERTY_CHARSET), e);
            }
        }
    }

}
