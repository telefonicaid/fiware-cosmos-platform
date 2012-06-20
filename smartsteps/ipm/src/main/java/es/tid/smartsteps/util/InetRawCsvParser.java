package es.tid.smartsteps.util;

import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;

/**
 * A CSV parser for INET-RAW CDRs.
 *
 * @author apv
 */
public class InetRawCsvParser extends CsvParserSupport<InetRaw> {

    private static class BuilderAdapter extends
            AbstractBuilderAdapter<InetRaw, InetRaw.Builder> {

        public BuilderAdapter(InetRaw.Builder builder) {
            super(builder);
        }

        @Override
        public InetRaw build() {
            return this.getBuilder().build();
        }
    }

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setType(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCallType(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setImsi(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setFirstTempImsi(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setLastTempImsi(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setImei(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setLacod(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCellId(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setEventDateTime(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setDtapCause(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setBssmapCause(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setCcCause(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setMmCause(fieldValue);
                }
            },
            new FieldParser<InetRaw, BuilderAdapter>() {
                @Override
                public void parseField(String fieldValue,
                                       BuilderAdapter adapter) {
                    adapter.getBuilder().setRanapCause(fieldValue);
                }
            },
    };

    public InetRawCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    protected FieldParser<InetRaw, Builder<InetRaw>>[] getFieldParsers() {
        return FIELD_PARSERS;
    }

    @Override
    protected Builder newBuilder() {
        return new BuilderAdapter(InetRaw.newBuilder());
    }
}
