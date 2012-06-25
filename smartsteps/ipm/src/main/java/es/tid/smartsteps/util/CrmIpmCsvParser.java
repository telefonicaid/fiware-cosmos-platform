package es.tid.smartsteps.util;

import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

/**
 * A CSV parser for CRM-IPM CDRs.
 *
 * @author apv
 */
public class CrmIpmCsvParser extends CsvParserSupport<CrmIpm> {

    private static class BuilderAdapter extends
            AbstractBuilderAdapter<CrmIpm, CrmIpm.Builder> {

        public BuilderAdapter(CrmIpm.Builder builder) {
            super(builder);
        }

        @Override
        public CrmIpm build() {
            return this.getBuilder().build();
        }
    }

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedMsisdn(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedBillingPostCode(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setBillingPostCodePrefix(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAcornCode(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setGender(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setBillingSystem(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setMtrcPlSegment(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setMpnStatus(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setSpid(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setActiveStatus(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setNeedsSegmentation(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAge(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAgeBand(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedImsi(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedImei(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setImeiTac(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setDeviceType(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setDeviceManufacturer(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setDeviceModelName(fieldValue);
                }
            },
            new FieldParser<CrmIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setEffectiveFromDate(fieldValue);
                }
            },
    };

    public CrmIpmCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    protected FieldParser<CrmIpm, Builder<CrmIpm>>[] getFieldParsers() {
        return FIELD_PARSERS;
    }

    @Override
    protected Builder newBuilder() {
        return new BuilderAdapter(CrmIpm.newBuilder());
    }
}
