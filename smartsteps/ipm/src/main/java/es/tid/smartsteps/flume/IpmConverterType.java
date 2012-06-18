package es.tid.smartsteps.flume;

import es.tid.smartsteps.ipm.RawToIpmConverter;

/**
 * An enumeration of known IPM converter types.
 * @author apv
 */
public enum IpmConverterType {
   CRM(es.tid.smartsteps.ipm.CrmRawToIpmConverter.class),
   INET(es.tid.smartsteps.ipm.InetRawToIpmConverter.class);

   private final Class<? extends RawToIpmConverter> converterClass;

   private IpmConverterType(Class<? extends RawToIpmConverter> converterClass) {
      this.converterClass = converterClass;
   }

   public Class<? extends RawToIpmConverter> getConverterClass() {
      return converterClass;
   }
}

