package es.tid.ps.mobility.parsing;

//import es.tid.analytics.mobility.core.data.Cdr;
import es.tid.ps.mobility.data.MobProtocol.Cdr;

/**
 * User: masp20
 * Date: 25-ene-2012
 * Time: 12:55:42
 */
public interface ParserCdr {
    public Cdr parseCdrsLine(final String line);
}
