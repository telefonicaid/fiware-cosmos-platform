package es.tid.smartsteps.util;

import java.io.InputStream;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

/**
*/
public class CrmIpmCsvParser extends CsvParserSupport<CrmIpm, CrmIpm.Builder> {

    private static final FieldParser[] fieldParsers = {
            // ANONYMISED_MSISDN
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedMsisdn(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedBillingPostCode(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAcornCode(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setBillingPostCodePrefix(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setGender(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setBillingSystem(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setMtrcPlSegment(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setMpnStatus(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setSpid(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setActiveStatus(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setNeedsSegmentation(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAge(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAgeBand(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedImsi(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedImei(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setImeiTac(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceType(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceManufacturer(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceModelName(fieldValue);
                }
            },
            new CsvParserSupport.FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setEffectiveFromDate(fieldValue);
                }
            },
    };

    public CrmIpmCsvParser(char delimiter, Charset charset) {
        super(delimiter, charset, fieldParsers);
    }

    @Override
    public CrmIpm parse(InputStream input) throws ParseException {
        CrmIpm.Builder builder = CrmIpm.newBuilder();
        this.parse(input, builder);
        return builder.build();
    }
}
