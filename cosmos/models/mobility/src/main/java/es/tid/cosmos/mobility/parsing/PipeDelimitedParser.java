package es.tid.cosmos.mobility.parsing;

import java.util.StringTokenizer;

import es.tid.cosmos.mobility.data.BaseProtocol;

/**
 *
 * @author sortega
 */
public abstract class PipeDelimitedParser extends StringTokenizer {
    protected static final String SEPARATOR = "|";
    protected static final int MAX_CELL_DIGITS = 8;
    protected static final int MAX_CLIENT_LENGTH = 10;

    public PipeDelimitedParser(String line) {
        super(line, SEPARATOR);
    }

    public abstract Object parse();

    protected void skipField() {
        nextToken();
    }

    protected int parseInt() {
        return Integer.parseInt(nextToken(), 10);
    }

    protected long parseLong() {
        return Long.parseLong(nextToken(), 10);
    }

    protected double parseDouble() {
        return Double.parseDouble(nextToken());
    }
    
    protected long parseCellId() {
        String str = nextToken();
        if (str.length() > MAX_CELL_DIGITS) {
            // Take the least significant hex digits
            str = str.substring(str.length() - MAX_CELL_DIGITS);
        }
        return Long.parseLong(str, 16);
    }

    protected long parseUserId() {
        String str = nextToken();
        if (str.length() > MAX_CLIENT_LENGTH) {
            str = str.substring(str.length() - MAX_CLIENT_LENGTH);
        }
        return Long.parseLong(str, 10);
    }

    protected BaseProtocol.Date parseDate() {
        String date = nextToken();
        final int day = Integer.parseInt(date.substring(0, 2), 10);
        final int month = Integer.parseInt(date.substring(3, 5), 10);
        final int year = Integer.parseInt(date.substring(8, 10), 10);
        return BaseProtocol.Date.newBuilder()
                .setDay(day)
                .setMonth(month)
                .setYear(year)
                .setWeekday(dayOfWeek(day, month, year))
                .build();
    }

    protected BaseProtocol.Time parseTime() {
        String time = nextToken();
        return BaseProtocol.Time.newBuilder()
                .setHour(Integer.parseInt(time.substring(0, 2), 10))
                .setMinute(Integer.parseInt(time.substring(3, 5), 10))
                .setSeconds(Integer.parseInt(time.substring(6, 8), 10))
                .build();
    }

    private static int dayOfWeek(int day, int month, int year) {
        int ix = ((year + 100 - 21) % 28) + monthOffset(month) + ((month > 2) ? 1 : 0);
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
