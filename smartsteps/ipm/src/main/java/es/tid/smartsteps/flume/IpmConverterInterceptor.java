package es.tid.smartsteps.flume;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.interceptor.Interceptor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import es.tid.cosmos.platform.injection.flume.AbstractInterceptor;
import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.RawToIpmConverter;

/**
 * A flume interceptor for converting incoming raw lines into IPM events.
 *
 * @author apv
 */
public class IpmConverterInterceptor extends AbstractInterceptor {
    private static final Logger LOGGER =
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
    public Event intercept(Event event) {
        try {
            ByteArrayInputStream oldBody = new ByteArrayInputStream(event
                    .getBody());
            ByteArrayOutputStream newBody = new ByteArrayOutputStream();
            this.converter.convert(oldBody, newBody);
            event.setBody(newBody.toByteArray());
            return event;
        } catch (ParseException e) {
            LOGGER.warn("event discarded due to parsing error", e);
            return null;
        } catch (IOException e) {
            LOGGER.warn("event discarded due to IO error", e);
            return null;
        }
    }

    public static class Builder implements Interceptor.Builder {

        private RawToIpmConverter converter = null;

        private static RawToIpmConverter.Builder getBuilderFromClass(
                String className) {
            String errorMessage = String.format("invalid value %s for '%s' property",
                    className, PROPERTY_CONVERTER);
            loadDerivedClass(className, RawToIpmConverter.class, errorMessage);
            Class<? extends RawToIpmConverter.Builder> factoryClass =
                    loadDerivedClass(className + "$Builder",
                            RawToIpmConverter.Builder.class,
                            errorMessage);
            try {
                return factoryClass.newInstance();
            } catch (Exception e) {
                throw new ConfigurationException(errorMessage + "; inner " +
                        "class Builder for given class cannot be " +
                        "instantiated", e);
            }
        }

        private static <Base, Derived> Class<Derived> loadDerivedClass(
                String className, Class<Base> baseClass, String errorMessage) {
            try {
                Class clazz = Class.forName(className);
                if (!baseClass.isAssignableFrom(clazz)) {
                    throw new ConfigurationException(String.format(
                            "%s; class %s is not derived from %s",
                            errorMessage, className, baseClass));
                }
                return (Class<Derived>) clazz;

            } catch (ClassNotFoundException e) {
                throw new ConfigurationException(String.format(
                        "%s; missing class %s", errorMessage, className), e);
            }
        }

        @Override
        public Interceptor build() {
            if (this.converter == null) {
                throw new IllegalStateException("cannot build IPM converter " +
                        "interceptor: the builder has not been initialized yet");
            }
            return new IpmConverterInterceptor(this.converter);
        }

        private static void checkPropertiesOrThrow(Context ctx) {
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
        public void configure(Context context) {
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
