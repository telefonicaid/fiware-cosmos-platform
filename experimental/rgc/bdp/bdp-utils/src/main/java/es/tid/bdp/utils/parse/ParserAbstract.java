package es.tid.bdp.utils.parse;

import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.google.protobuf.Message;

/**
 *
 * @author rgc
 *
 */
public abstract class ParserAbstract {

    private static final String DEFAULT_SEPARATOR = "\\|";

    protected Pattern pattern;
    protected Matcher matcher;
    protected Map<String, Integer> regPosition;

    public ParserAbstract(final String pattern, final String attr) {
        this(pattern, attr, DEFAULT_SEPARATOR);
    }

    public ParserAbstract(final String pattern, final String attr,
            final String separator) {
        init(pattern, attr, separator);
    }

    /**
     * It is a constructor that prepares the pattern and the structures for
     * parsing the data.
     */
    protected void init(final String pattern, final String attr,
            final String separator) {
        this.pattern = Pattern.compile(pattern);
        this.matcher = this.pattern.matcher("");

        this.regPosition = new HashMap<String, Integer>();
        String[] array = attr.split(separator);
        for (int i = 0; i < array.length; i++) {
            this.regPosition.put(array[i], i + 1);
        }
    }

    /**
     * Method that parses a hexadecimal string to a Long. If the string is empty
     * return 0L
     *
     * @param strNum
     *            hexadecimal string for parsing.
     * @return a long the sting value parsed
     */
    protected Long parserHexNumber(final String strNum) {
        if (strNum.isEmpty()) {
            return 0L;
        }
        return Long.parseLong(strNum, 16);
    }

    /**
     * Method parses a string and return a Dynamic Protocol buffer message
     *
     * @param line
     *            row data for parsing it
     * @return a encoding data in a Dynamic Protocol buffer message
     * @throws RuntimeException
     *             throws an exception if the line does not match with the
     *             regular expression
     */
    public Message parseLine(final String line) {
        this.matcher.reset(line);
        if (this.matcher.matches()) {
            return createMessage();
        } else {
            throw new RuntimeException("no matches");
        }
    }

    /**
     * This abstract method is used for create new message.
     *
     * @param matcher
     *            Object that will match the given input against this pattern
     * @return a encoding data in a Dynamic Protocol buffer message
     */
    protected abstract Message createMessage();

}
