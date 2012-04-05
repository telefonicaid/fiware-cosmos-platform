package es.tid.bdp.mobility.parsing;

import java.text.ParseException;
import java.text.SimpleDateFormat;

import org.apache.log4j.Logger;

import es.tid.bdp.mobility.data.Cell;
import es.tid.bdp.mobility.data.BaseProtocol.Date;
import es.tid.bdp.mobility.data.BaseProtocol.Time;
import es.tid.bdp.mobility.data.MobProtocol.Cdr;

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
        Cdr.Builder cdr = Cdr.newBuilder();
        Date.Builder date = Date.newBuilder();
        Time.Builder time = Time.newBuilder();
        try {
            final String[] lineParsed = line.split("[|]");
            if (lineParsed != null && lineParsed.length == CDR_FIELD_MAX) {
                cdr.setUserId(Long.parseLong(lineParsed[1]));
                final long locationIni = Long.parseLong(lineParsed[0]);
                final long locationFin = Long.parseLong(lineParsed[2]);
                cdr.setCellId(locationIni > 0 ? locationIni : locationFin);
                final String dateString = lineParsed[6] + "-" + lineParsed[7];
                java.util.Date dateAux = DATE_FORMAT.parse(dateString);
                date.setDay(dateAux.getDate());
                date.setMonth(dateAux.getMonth()+1);
                date.setYear(dateAux.getYear()+1900);
                date.setWeekday(dateAux.getDay()+1);
                cdr.setDate(date.build());
                time.setHour(dateAux.getHours()%24);
                time.setMinute(dateAux.getMinutes());
                time.setSeconds(dateAux.getSeconds());
                cdr.setTime(time.build());
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
        return cdr.build();
    }

}