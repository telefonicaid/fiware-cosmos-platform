package es.tid.smartsteps.util;

import java.io.InputStream;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;

/**
 */
public class CrmRawCsvParser extends CsvParserSupport<CrmRaw, CrmRaw.Builder> {

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setMsisdn(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setBillingPostCode(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setAcornCode(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setGender(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setBillingSystem(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setMtrcPlSegment(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setMpnStatus(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setSpid(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setActiveStatus(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setNeedsSegmentation(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setAge(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setAgeBand(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setImsi(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setImei(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setDeviceType(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setDeviceManufacturer(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setDeviceModelName(fieldValue);
                }
            },
            new FieldParser<CrmRaw.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmRaw.Builder builder) {
                    builder.setEffectiveFromDate(fieldValue);
                }
            },
    };

    public CrmRawCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public CrmRaw parse(InputStream input) throws ParseException {
        CrmRaw.Builder builder = CrmRaw.newBuilder();
        this.parse(input, builder, FIELD_PARSERS);
        return builder.build();
    }
}
