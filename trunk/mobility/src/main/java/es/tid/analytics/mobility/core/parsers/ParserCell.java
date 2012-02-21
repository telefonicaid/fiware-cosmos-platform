package es.tid.analytics.mobility.core.parsers;

import es.tid.analytics.mobility.core.data.Cell;


/**
 * User: masp20
 * Date: 25-ene-2012
 * Time: 12:37:12
 */
public interface ParserCell {
    public Cell parseCellLine(final String line);
}
