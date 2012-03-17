package es.tid.bdp.utils.parse;

import java.io.IOException;
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

    private static final String SEPARATOR = "\\|";

    protected Pattern pattern;
    protected Map<String, Integer> regPosition;

    public ParserAbstract(String pattern, String attr) {
        init(pattern, attr);
    }

    /**
     * It is a constructor that prepares the pattern and the structures for
     * parsing the data.
     * 
     * @throws IOException
     */
    protected void init(String pattern, String attr) {
        this.pattern = Pattern.compile(pattern);

        this.regPosition = new HashMap<String, Integer>();
        String[] array = attr.split(SEPARATOR);
        for (int i = 0; i < array.length; i++) {
            this.regPosition.put(array[i], i + 1);
        }
    }

    /**
     * Method parses a string and return a Dynamic Protocol buffer message
     * 
     * @param line
     *            row data for parsing it
     * @return a encoding data in a Dynamic Protocol buffer message
     */
    public Message parseLine(String line) {
        Matcher m = this.pattern.matcher(line);
        if (m.matches()) {
            return createMessage(m);
        } else {
            throw new RuntimeException("no matches");
        }
    }

    protected abstract Message createMessage(Matcher matcher);
}
