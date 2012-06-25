package es.tid.smartsteps.util;

import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;

/**
 * A CSV parser for INET-IPM CDRs.
 *
 * @author apv
 */
public class InetIpmCsvParser extends CsvParserSupport<InetIpm> {

    private static class BuilderAdapter extends
            AbstractBuilderAdapter<InetIpm, InetIpm.Builder> {

        public BuilderAdapter(InetIpm.Builder builder) {
            super(builder);
        }

        @Override
        public InetIpm build() {
            return this.getBuilder().build();
        }
    }

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setType(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCallType(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedImsi(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setFirstTempImsi(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setLastTempImsi(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setImeiTac(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setAnonymisedImei(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setLacod(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCellId(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setEventDateTime(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setDtapCause(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setBssmapCause(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCcCause(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setMmCause(fieldValue);
                }
            },
            new FieldParser<InetIpm, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setRanapCause(fieldValue);
                }
            },
    };

    public InetIpmCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    protected FieldParser<InetIpm, Builder<InetIpm>>[] getFieldParsers() {
        return FIELD_PARSERS;
    }

    @Override
    protected Builder newBuilder() {
        return new BuilderAdapter(InetIpm.newBuilder());
    }
}
