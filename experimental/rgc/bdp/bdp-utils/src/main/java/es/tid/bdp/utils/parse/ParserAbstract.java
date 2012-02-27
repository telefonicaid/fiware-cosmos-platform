package es.tid.bdp.utils.parse;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
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
     * @param cdrLine
     *            row data for parsing it
     * @return a encoding data in a Dynamic Protocol buffer message
     */
    public abstract Message parseLine(String cdrLine);

}
