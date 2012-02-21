package es.tid.analytics.mobility.core.parsers;

import org.apache.log4j.Logger;

import es.tid.analytics.mobility.core.data.Cdr;
import es.tid.analytics.mobility.core.data.Cell;

import java.text.ParseException;
import java.text.SimpleDateFormat;

/**
 * User: masp20
 * Date: 25-ene-2012
 * Time: 12:40:07
 */
public class ParserDefault implements ParserCell, ParserCdr {

    private static final Logger LOG = Logger.getLogger(ParserDefault.class);

    protected static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMdd-HH:mm:ss");
    protected static final SimpleDateFormat OLD_DATE_FORMAT = new SimpleDateFormat("dd/MM/yyyy-HH:mm:ss");

    protected static final int CELL_FIELD_MAX = 6;
    protected static final int CDR_FIELD_MAX = 15;


    public ParserDefault() {
        DATE_FORMAT.setLenient(false);
    }

    @Override
    public Cell parseCellLine(final String line) {
        final Cell cell = new Cell();
        try {
            final String[] lineParsed = line.split("[|]");
            if (lineParsed != null && lineParsed.length == CELL_FIELD_MAX) {
                cell.setGeoLocationLevel1(Long.parseLong(lineParsed[0]));
                cell.setGeoLocationLevel2(Long.parseLong(lineParsed[1]));
                cell.setGeoLocationLevel3(Long.parseLong(lineParsed[2]));
                cell.setGeoLocationLevel4(Long.parseLong(lineParsed[3]));
                cell.setLatitude(Double.parseDouble(lineParsed[4]));
                cell.setLongitude(Double.parseDouble(lineParsed[5]));

                calculateCellId(cell);

            } else {
                LOG.debug("Line rejected: " + line);
            }
        } catch (NumberFormatException e) {
            LOG.debug("Line rejected: " + line);
            return null;
        }
        return cell;
    }

    protected void calculateCellId(final Cell cell) {
        switch (ParserFactory.GEO_LOCATION_LEVEL) {
            case 1: {
                cell.setIdCell(cell.getGeoLocationLevel1());
                break;
            }
            case 2: {
                cell.setIdCell(cell.getGeoLocationLevel2());
                break;
            }
            case 3: {
                cell.setIdCell(cell.getGeoLocationLevel3());
                break;
            }
            case 4: {
                cell.setIdCell(cell.getGeoLocationLevel4());
                break;
            }
            default: {
                cell.setIdCell(cell.getGeoLocationLevel1());
                break;
            }
        }
    }

    @Override
    public Cdr parseCdrsLine(final String line) {
        final Cdr cdr = new Cdr();
        try {
            final String[] lineParsed = line.split("[|]");
            if (lineParsed != null && lineParsed.length == CDR_FIELD_MAX) {
                cdr.setNode(Long.parseLong(lineParsed[1]));
                final long locationIni = Long.parseLong(lineParsed[0]);
                final long locationFin = Long.parseLong(lineParsed[2]);
                cdr.setIdCell(locationIni > 0 ? locationIni : locationFin);
                final String dateString = lineParsed[6] + "-" + lineParsed[7];
                cdr.setDate(DATE_FORMAT.parse(dateString));
            } else {
                LOG.debug("Line rejected: " + line);
            }
        } catch (ParseException e) {
            LOG.debug("Line rejected: " + line);
            return null;
        } catch (NumberFormatException e) {
            LOG.debug("Line rejected: " + line);
            return null;
        }
        return cdr;
    }

}