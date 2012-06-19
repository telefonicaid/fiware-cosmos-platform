package es.tid.smartsteps.flume;

import es.tid.smartsteps.ipm.CrmRawToIpmConverter;
import es.tid.smartsteps.ipm.InetRawToIpmConverter;
import es.tid.smartsteps.ipm.RawToIpmConverter;

/**
 * An enumeration of known IPM converter types.
 *
 * @author apv
 */
public enum IpmConverterType {
    CRM(es.tid.smartsteps.ipm.CrmRawToIpmConverter.class,
            new CrmRawToIpmConverter.Builder()),
    INET(es.tid.smartsteps.ipm.InetRawToIpmConverter.class,
            new InetRawToIpmConverter.Builder());

    private final Class<? extends RawToIpmConverter> converterClass;
    private final RawToIpmConverter.Builder converterBuilder;

    private IpmConverterType(Class<? extends RawToIpmConverter> converterClass,
                             RawToIpmConverter.Builder converterBuilder) {
        this.converterClass = converterClass;
        this.converterBuilder = converterBuilder;
    }

    public Class<? extends RawToIpmConverter> getConverterClass() {
        return this.converterClass;
    }

    public RawToIpmConverter.Builder getConverterBuilder() {
        return converterBuilder;
    }
}

