package es.tid.ps.mobility.parsers;

import es.tid.ps.mobility.data.Cdr;


/**
 * User: masp20
 * Date: 25-ene-2012
 * Time: 12:55:42
 */
public interface ParserCdr {
    public Cdr parseCdrsLine(final String line);
}
