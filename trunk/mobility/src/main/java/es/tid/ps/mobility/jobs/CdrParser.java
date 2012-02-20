package es.tid.ps.mobility.jobs;

import java.util.StringTokenizer;

import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author sortega
 */
public class CdrParser extends StringTokenizer {
    private static final String SEPARATOR = "|";
    private static final int MAX_CELL_DIGITS = 8;
    private static final int MAX_CLIENT_LENGTH = 10;

    public CdrParser(String line) {
        super(line, SEPARATOR);
    }

    //LINE --> "33F430521676F4|2221436242|33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR"
    public MxCdr parse() {
        MxCdr.Builder cdr = MxCdr.newBuilder();

        cdr.setCell(parseCell()); // Cell 1
        cdr.setPhone(parsePhone()); // Phone 1
        if (cdr.getCell() == 0) { // Cell 2
            cdr.setCell(parseCell());
        } else {
            skipField();
        }
        skipField(); // Phone 2
        skipField(); // Unknown field

        cdr.setDate(parseDate());
        cdr.setTime(parseTime());

        return cdr.build();
    }

    private void skipField() {
        nextToken();
    }

    private long parseCell() throws NumberFormatException {
        String str = nextToken();
        if (str.length() > MAX_CELL_DIGITS) {
            // Take the least significant hex digits
            str = str.substring(str.length() - MAX_CELL_DIGITS);
        }
        return Long.parseLong(str, 16);
    }

    private long parsePhone() {
        String str = nextToken();
        if (str.length() > MAX_CLIENT_LENGTH) {
            str = str.substring(str.length() - MAX_CLIENT_LENGTH);
        }
        return Long.parseLong(str, 10);
    }

    private Date parseDate() {
        String date = nextToken();
        final int day = Integer.parseInt(date.substring(0, 2), 10);
        final int month = Integer.parseInt(date.substring(3, 5), 10);
        final int year = Integer.parseInt(date.substring(8, 10), 10);
        return Date.newBuilder().setDay(day).setMonth(month).setYear(year).
                setWeekDay(dayOfWeek(day, month, year)).build();
    }

    private Time parseTime() {
        String time = nextToken();
        return Time.newBuilder()
                .setHour(Integer.parseInt(time.substring(0, 2), 10))
                .setMinute(Integer.parseInt(time.substring(3, 5), 10))
                .setSeconds(Integer.parseInt(time.substring(6, 8), 10))
                .build();
    }

    private static int dayOfWeek(int day, int month, int year) {
        year += 100; // real year + 2000 - 1900
        int ix = ((year - 21) % 28) + monthOffset(month) + ((month > 2) ? 1 : 0);
        int tx = (ix + (ix / 4)) % 7 + day;
        return (tx + 1) % 7;
    }

    private static int monthOffset(int month) {
        switch (month) {
            case 2: case 6:
                return 0;
            case 8:
                return 4;
            case 10:
                return 8;
            case 9: case 12:
                return 12;
            case 3: case 11:
                return 16;
            case 1: case 5:
                return 20;
            case 4: case 7:
                return 24;
            default:
                throw new IllegalArgumentException("Not a month");
        }
    }
}
