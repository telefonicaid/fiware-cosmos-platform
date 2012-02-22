package es.tid.ps.mobility.parsing;

import org.apache.log4j.Logger;

import java.util.HashMap;

/**
 * User: masp20 Date: 25-ene-2012 Time: 12:36:11
 */
public class ParserFactory {
    private static final Logger LOG = Logger.getLogger(ParserFactory.class);
    private static final String PARSER_DEFAULT_CLASS_NAME =
            es.tid.ps.mobility.parsing.ParserDefault.class.getName();
    private static final String PARSER_ALPHA_CLASS_NAME =
            es.tid.ps.mobility.parsing.ParserAlphanumeric.class.getName();
    public static final String PARSER_DEFAULT = "DEFAULT";
    public static final String PARSER_ALPHA = "ALPHA";
    public static final int GEO_LOCATION_LEVEL = 1;
    public static final HashMap<String, String> PARSER_CONTAINER =
            new HashMap<String, String>();

    public ParserFactory() {
        ParserFactory.loadParsers();
    }

    private static void loadParsers() {
        ParserFactory.PARSER_CONTAINER.put(ParserFactory.PARSER_DEFAULT,
                                           ParserFactory.PARSER_DEFAULT_CLASS_NAME);
        ParserFactory.PARSER_CONTAINER.put(ParserFactory.PARSER_ALPHA,
                                           ParserFactory.PARSER_ALPHA_CLASS_NAME);
    }

    public ParserCell createNewDefaultCellParser() {
        ParserCell parser = null;
        try {
            parser = (ParserCell) Class.forName(
                    ParserFactory.PARSER_DEFAULT_CLASS_NAME).newInstance();
        } catch (InstantiationException ie) {
            LOG.error("ERROR al instanciar el parser", ie);
        } catch (IllegalAccessException iae) {
            LOG.error("ERROR al instanciar el parser", iae);
        } catch (ClassNotFoundException cnfe) {
            LOG.error("ERROR al instanciar el parser", cnfe);
        }

        return parser;
    }

    public ParserCdr createNewDefaultCdrParser() {
        ParserCdr parser = null;
        try {
            parser = (ParserCdr) Class.forName(
                    ParserFactory.PARSER_DEFAULT_CLASS_NAME).newInstance();
        } catch (InstantiationException ie) {
            LOG.error("ERROR al instanciar el parser", ie);
        } catch (IllegalAccessException iae) {
            LOG.error("ERROR al instanciar el parser", iae);
        } catch (ClassNotFoundException cnfe) {
            LOG.error("ERROR al instanciar el parser", cnfe);
        }

        return parser;
    }

    public ParserCell createNewCellParser(final String parserName) {
        ParserCell parser = null;

        try {
            final String parserClassName = PARSER_CONTAINER.get(parserName);
            parser = (ParserCell) Class.forName(parserClassName).newInstance();
        } catch (InstantiationException ie) {
            LOG.error("ERROR al instanciar el parser", ie);
        } catch (IllegalAccessException iae) {
            LOG.error("ERROR al instanciar el parser", iae);
        } catch (ClassNotFoundException cnfe) {
            LOG.error("ERROR al instanciar el parser", cnfe);
        }

        return parser;
    }

    public ParserCdr createNewCdrParser(final String parserName) {
        ParserCdr parser = null;

        try {
            final String parserClassName = PARSER_CONTAINER.get(parserName);
            parser = (ParserCdr) Class.forName(parserClassName).newInstance();
        } catch (InstantiationException ie) {
            LOG.error("ERROR al instanciar el parser", ie);
        } catch (IllegalAccessException iae) {
            LOG.error("ERROR al instanciar el parser", iae);
        } catch (ClassNotFoundException cnfe) {
            LOG.error("ERROR al instanciar el parser", cnfe);
        }

        return parser;
    }
}
