package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
import java.util.StringTokenizer;

import com.google.protobuf.Message;
import org.apache.commons.io.IOUtils;

import es.tid.smartsteps.ipm.ParseException;

/**
 */
public abstract class CsvParserSupport<T, Builder extends Message.Builder>
        extends AbstractCsvParser<T> {

    public static interface FieldParser<Builder extends Message.Builder> {
        void parseField(String fieldValue,
                        Builder builder) throws ParseException;
    }

    protected CsvParserSupport(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    protected void parse(InputStream input, Builder builder,
                         FieldParser[] fieldParsers) throws ParseException {
        String encoding = this.getCharset().name();
        try {
            String line = IOUtils.toString(input, this.getCharset().name());
            StringTokenizer st = new StringTokenizer(line,
                    this.getDelimiter());
            int fieldsCount = st.countTokens();
            if (fieldsCount != fieldParsers.length) {
                throw new ParseException(String.format(
                        "cannot parse input line:\n   %s\nexpected %d " +
                                "columns but %d was found",
                        line, fieldParsers.length, fieldsCount));
            }
            for (int i = 0; i < fieldsCount; i++) {
                fieldParsers[i].parseField(st.nextToken(), builder);
            }
        } catch (IOException e) {
            throw new ParseException(String.format("cannot read a string line" +
                    " from input with given charset %s", encoding), e);
        }
    }
}
