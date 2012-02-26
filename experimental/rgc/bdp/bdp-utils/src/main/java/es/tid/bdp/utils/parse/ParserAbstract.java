package es.tid.bdp.utils.parse;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

import com.google.protobuf.Message;

import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * 
 * @author rgc
 * 
 */
public abstract class ParserAbstract {

    private static final String INPUT_ATTRS_REGEX = "input.attrs.regex";
    private static final String INPUT_ATTRS_DESC = "input.attrs.desc";
    private static final String SEPARATOR = "\\|";

    protected Pattern pattern;
    protected Map<String, Integer> regPosition;

    public ParserAbstract() throws IOException {
        init();

    }

    /**
     * It is a constructor that prepares the pattern and the structures for
     * parsing the data.
     * 
     * @throws IOException
     */
    private void init() throws IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        this.pattern = Pattern.compile(properties
                .getProperty(INPUT_ATTRS_REGEX));

        this.regPosition = new HashMap<String, Integer>();
        String[] array = properties.getProperty(INPUT_ATTRS_DESC).split(
                SEPARATOR);
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
