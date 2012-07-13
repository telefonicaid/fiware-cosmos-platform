package es.tid.smartsteps.util;

import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;

/**
 * A CSV parser for CRM-RAW CDRs.
 *
 * @author apv
 */
public class CrmRawCsvParser extends CsvParserSupport<CrmRaw> {

    private static class BuilderAdapter extends
            AbstractBuilderAdapter<CrmRaw, CrmRaw.Builder> {

        public BuilderAdapter(CrmRaw.Builder builder) {
            super(builder);
        }

        @Override
        public CrmRaw build() {
            return getBuilder().build();
        }
    }

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setMsisdn(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setBillingPostCode(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setAcornCode(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setGender(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setBillingSystem(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setMtrcPlSegment(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setMpnStatus(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setSpid(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setActiveStatus(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setNeedsSegmentation(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setAge(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setAgeBand(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setImsi(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setImei(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setDeviceType(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setDeviceManufacturer(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setDeviceModelName(fieldValue);
                }
            },
            new FieldParser<CrmRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter ) {
                    adapter.getBuilder().setEffectiveFromDate(fieldValue);
                }
            },
    };

    public CrmRawCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    protected FieldParser<CrmRaw, Builder<CrmRaw>>[] getFieldParsers() {
        return FIELD_PARSERS;
    }

    @Override
    protected Builder newBuilder() {
        return new BuilderAdapter(CrmRaw.newBuilder());
    }
}
