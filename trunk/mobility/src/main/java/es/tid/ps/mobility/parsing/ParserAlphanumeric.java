package es.tid.ps.mobility.parsing;

import es.tid.ps.mobility.data.Cell;
import es.tid.ps.mobility.data.BaseProtocol;
import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MobProtocol.Cdr;

import org.apache.log4j.Logger;

import java.text.ParseException;


/**
 * User: masp20
 * Date: 26-ene-2012
 * Time: 13:45:34
 */
public class ParserAlphanumeric extends ParserDefault {


    private static final int BASE = 37;

    /**
     * Default values for BASE raised to the power of i (BASE^0, BASE^1...)
     */
    private static long[] FIELD_POW_BASE = {(long) Math.pow(BASE, 0), (long) Math.pow(BASE, 1), (long) Math.pow(BASE, 2), (long) Math.pow(BASE, 3),
                                            (long) Math.pow(BASE, 4), (long) Math.pow(BASE, 5), (long) Math.pow(BASE, 6), (long) Math.pow(BASE, 7),
                                            (long) Math.pow(BASE, 8), (long) Math.pow(BASE, 9), (long) Math.pow(BASE, 10), (long) Math.pow(BASE, 11)
    };

    private static final Logger LOG = Logger.getLogger(ParserDefault.class);
    public static final int MAX_FIELD_DIGIT = 12;


    @Override
    public Cell parseCellLine(final String line) {
        final Cell cell = new Cell();
        try {
            final String[] lineParsed = line.split("[|]");
            if (lineParsed != null && lineParsed.length == 6) {
                cell.setGeoLocationLevel1(gstCodedStrToLong(lineParsed[0], MAX_FIELD_DIGIT));
                cell.setGeoLocationLevel2(gstCodedStrToLong(lineParsed[1], MAX_FIELD_DIGIT));
                cell.setGeoLocationLevel3(gstCodedStrToLong(lineParsed[2], MAX_FIELD_DIGIT));
                cell.setGeoLocationLevel4(gstCodedStrToLong(lineParsed[3], MAX_FIELD_DIGIT));
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

    
    @Override
    public Cdr parseCdrsLine(final String line) {
        Cdr.Builder cdr = Cdr.newBuilder();
        Date.Builder date = BaseProtocol.Date.newBuilder();
        Time.Builder time = BaseProtocol.Time.newBuilder();
        
        try {
            final String[] lineParsed = line.split("[|]");
            if (lineParsed != null && lineParsed.length == CDR_FIELD_MAX) {
                cdr.setUserId(Long.parseLong(lineParsed[1]));
                final long locationIni = gstCodedStrToLong(lineParsed[0], MAX_FIELD_DIGIT);
                final long locationFin = gstCodedStrToLong(lineParsed[2], MAX_FIELD_DIGIT);
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


    /**
     * Function to coded a char into decimal
     *
     * @param line - character to traslate.
     * @return - char traslated to int
     */
    private static int charCoded2int(final char line) {
        if (line >= 48 && line <= 57) {
            return (line - 47);
        } else if (line >= 65 && line <= 90) {
            return (line - 54);
        } else if (line >= 97 && line <= 122) {
            return (line - 86);
        } else {
            return 0;
        }

    }


    /**
     * Function to decode a decimal into char.
     *
     * @param line - int to traslate
     * @return - int traslated to char
     */
    private static char intCoded2char(final long line) {
        if (line >= 1 && line <= 10) {
            return (char) (line + 47);
        } else if (line >= 11 && line <= 36) {
            return (char) (line + 54);
        } else {
            return ' ';
        }
    }


    /**
     * Function to encode a alfanumeric cell id into an integer cell id.
     *
     * @param strId     - the string cell id.
     * @param maxLength - Max number of c
     * @return long -
     */
    public static long gstCodedStrToLong(final String strId, final int maxLength) {
        final int fieldLength;
        long id;
        int posValue;

        fieldLength = strId.length();
        id = 0;

        if (fieldLength > maxLength) {
            for (int i = 0; i < maxLength; i++) {
                posValue = charCoded2int(strId.charAt(i));
                id += posValue * FIELD_POW_BASE[maxLength - i - 1];
            }
        } else {
            for (int i = 0; i < fieldLength; i++) {
                posValue = charCoded2int(strId.charAt(i));
                id += posValue * FIELD_POW_BASE[fieldLength - i - 1];
            }
        }

        LOG.debug("Field " + strId + " -> " + id);
        return id;
    }

    private static int codedIntLength(final long field) {
        int i;
        for (i = 0; i < MAX_FIELD_DIGIT; i++) {
            if (field < FIELD_POW_BASE[i]) {
                break;
            }
        }
        return i;
    }


    /**
     * Function to decode an integer id into an alphanumeric id.
     *
     * @param id - the integer id to decode
     * @return - the string decoded
     */
    public static String gstDecodeAphaIntToStr(final long id) {
        final int fieldLength;
        final String result;

        fieldLength = codedIntLength(id);
        final char[] strId = new char[fieldLength];

        long reminder = id;

        for (int i = 0; i < fieldLength; i++) {
            strId[fieldLength - i - 1] = intCoded2char(reminder % BASE);
            reminder = (reminder / BASE);
        }

        result = String.copyValueOf(strId);

        LOG.debug(id + " -> " + result);
        return result;
    }

}
