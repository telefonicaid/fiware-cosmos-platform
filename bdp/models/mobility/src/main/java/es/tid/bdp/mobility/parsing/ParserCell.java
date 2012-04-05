package es.tid.bdp.mobility.parsing;

import es.tid.bdp.mobility.data.Cell;


/**
 * User: masp20
 * Date: 25-ene-2012
 * Time: 12:37:12
 */
public interface ParserCell {
    public Cell parseCellLine(final String line);
}
