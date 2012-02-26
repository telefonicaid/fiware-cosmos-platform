package es.tid.bdp.utils.parse;

import java.io.IOException;
import java.util.regex.Matcher;

import com.google.protobuf.Message;

import es.tid.bdp.profile.data.CdrP.Cdr;
import es.tid.bdp.profile.data.CdrP.Date;
import es.tid.bdp.profile.data.CdrP.Time;

/**
 * This class parses a row line into the Cdr data structure of the Big Data
 * Platform. The data is wrapper into a Protocol Buffer Writable Structure.
 * 
 * @author rgc
 * 
 */
public class ParserCdr extends ParserAbstract {

    private final String ATTR_CELL_ID = "cellId";
    private final String ATTR_USER_ID = "userId";
    private final String ATTR_TIME_HOUR = "hour";
    private final String ATTR_TIME_MIN = "minute";
    private final String ATTR_TIME_SEC = "second";
    private final String ATTR_DATE_DAY = "day";
    private final String ATTR_DATE_MONTH = "month";
    private final String ATTR_CELL_YEAR = "year";      

    public ParserCdr() throws IOException {
        super();
    }

    /*
     * (non-Javadoc)
     * 
     * @see es.tid.bdp.sftp.io.ParserAbstract#parseLine(java.lang.String)
     */
    @Override
    public Message parseLine(String cdrLine) {
        Matcher m = this.pattern.matcher(cdrLine);
        if (m.matches()) {
            return createCdr(m);
        } else {
            throw new RuntimeException("no matches");
        }
    }

    private Cdr createCdr(Matcher m) {
        Cdr.Builder builder = Cdr.newBuilder();
        builder.setCellId(Long.parseLong(
                m.group(regPosition.get(ATTR_USER_ID)), 16));
        try {
            builder.setUserId(Long.parseLong(
                    m.group(regPosition.get(ATTR_CELL_ID)), 16));
        } catch (NumberFormatException e) {
            builder.setUserId(0L);
        }
        builder.setDate(parseDate(m));
        builder.setTime(parseTime(m));

        return builder.build();
    }

    /**
     * this method parses the
     * 
     * @param m
     * @return
     */
    private Time parseTime(Matcher m) {
        return Time
                .newBuilder()
                .setHour(
                        Integer.parseInt(m.group(regPosition
                                .get(ATTR_TIME_HOUR))))
                .setMinute(
                        Integer.parseInt(m.group(regPosition.get(ATTR_TIME_MIN))))
                .setSeconds(
                        Integer.parseInt(m.group(regPosition.get(ATTR_TIME_SEC))))
                .build();
    }

    private Date parseDate(Matcher m) {
        final int day = Integer
                .parseInt(m.group(regPosition.get(ATTR_DATE_DAY)));
        final int month = Integer.parseInt(m.group(regPosition
                .get(ATTR_DATE_MONTH)));
        final int year = Integer.parseInt(m.group(regPosition
                .get(ATTR_CELL_YEAR)));

        return Date.newBuilder().setDay(day).setMonth(month).setYear(year)
                .setWeekday(dayOfWeek(day, month, year)).build();
    }

    /**
     * This method generates the day of week front a date. It needs the day
     * month and year and generates it.
     * 
     * @param day
     *            the day of the date
     * @param month
     *            the month of the date
     * @param year
     *            the year of the date
     * @return a integer who represents the day of the week
     */
    private int dayOfWeek(int day, int month, int year) {
        year += 100; // real year + 2000 - 1900
        int ix = ((year - 21) % 28) + monthOffset(month)
                + ((month > 2) ? 1 : 0);
        int tx = (ix + (ix / 4)) % 7 + day;
        return (tx + 1) % 7;
    }

    /**
     * Method that generates the off set of each month for completing a week.
     * 
     * @param month
     *            the month
     * @return the offset
     */
    private int monthOffset(int month) {
        switch (month) {
        case 2:
        case 6:
            return 0;
        case 8:
            return 4;
        case 10:
            return 8;
        case 9:
        case 12:
            return 12;
        case 3:
        case 11:
            return 16;
        case 1:
        case 5:
            return 20;
        case 4:
        case 7:
            return 24;
        default:
            throw new IllegalArgumentException("Not a month");
        }
    }
}
