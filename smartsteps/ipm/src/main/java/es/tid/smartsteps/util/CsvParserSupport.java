package es.tid.smartsteps.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

import es.tid.smartsteps.ipm.ParseException;

/** CSV parser support. This class supports the actual implementation of a CSV
 * parser. It defines a {@link #parse(java.io.InputStream)} parse method} which
 * does the real parsing supported by a set of template methods for gathering
 * the required objects. <p/>
 *
 * The {@link es.tid.smartsteps.util.CsvParserSupport.FieldParser} inner
 * interface defines the signature of an object capable of parsing independent
 * fields of the CSV input. The {@link #parse(java.io.InputStream)} obtains an
 * array of {@link FieldParser} objects via {@link #getFieldParsers ()}
 * template method, assuming that the n-th parser corresponds to the n column
 * of each CSV line. Each field parser receives the column value and a fluent
 * builder as input, incarnated by {@link Builder} interface. It must parse
 * the given value and use it to fill the corresponding property using the
 * given fluent builder. <p/>
 *
 * In summary, any concrete CSV parser that uses this support class must
 * implement the following template methods.
 * <ul>
 *     <li>{@link #getFieldParsers()}, which provides the array of field
 *     parsers used to parse each column of CSV input.</li>
 *     <li>{@link #newBuilder()}, which provides a new builder object used to
 *     build a T instance
 *     </li>
 * </ul>
 *
 * @author apv
 */
public abstract class CsvParserSupport<T> extends AbstractCsvParser<T> {

    public interface Builder<T> {
        T build();
    }

    public abstract static class AbstractBuilderAdapter<T,
            Delegate> implements Builder<T> {

        private Delegate builder;

        public AbstractBuilderAdapter(Delegate builder) {
            this.builder = builder;
        }

        public Delegate getBuilder() {
            return builder;
        }
    }

    public interface FieldParser<T, B extends Builder<T>> {
        void parseField(String fieldValue, B builder)
                throws ParseException;
    }

    protected CsvParserSupport(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public List<T> parse(InputStream input) throws ParseException, IOException {
        FieldParser<T, Builder<T>>[] fieldParsers = this.getFieldParsers();
        List<T> result = new LinkedList<T>();

        BufferedReader reader = new BufferedReader(new InputStreamReader(
                input, this.getCharset()));
        String line;
        while ((line = reader.readLine()) != null) {
            Builder<T> builder = this.newBuilder();
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
            result.add(builder.build());
        }
        return result;
    }

    protected abstract FieldParser<T, Builder<T>>[] getFieldParsers();

    protected abstract Builder newBuilder();
}
