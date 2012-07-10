package es.tid.cosmos.mobility.parsing;

import static java.lang.Integer.parseInt;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;

/**
 * Parses a BaseProtocol.Date based on a limited date format string.
 *
 * Date fields are specified by a number of contiguous characters. Any other
 * character is interpreted as a field separator and ignored. Supported fields
 * are:
 * <ul>
 *     <li>'y' year field</li>
 *     <li>'m' month field</li>
 *     <li>'d' day field</li>
 * </ul>
 *
 * Example use:
 *
 * <code>
 *     DateParser parser = new DateParser("yyyy-mm-dd");
 *     parser.parse("2012-05-04");
 * </code>
 *
 * @author sortega
 */
public class DateParser {

    private static final int DEFAULT_RADIX = 10;

    private final StringRange yearRange;
    private final StringRange monthRange;
    private final StringRange dayRange;

    public DateParser(String dateFormat) {
        this.yearRange = findRange(dateFormat, 'y');
        this.monthRange = findRange(dateFormat, 'm');
        this.dayRange = findRange(dateFormat, 'd');
    }

    private StringRange findRange(String dateFormat, char formatChar) {
        int start = dateFormat.indexOf(formatChar);
        if (start < 0) {
            throw new IllegalArgumentException("Missing '" + formatChar +
                                               "' field");
        }
        int end = dateFormat.lastIndexOf(formatChar);
        StringRange range = new StringRange(start, end);
        for (char rangeChar : range.select(dateFormat).toCharArray()) {
            if (rangeChar != formatChar) {
                throw new IllegalArgumentException("Repeated field '" +
                                                   formatChar + "'");
            }
        }
        return range;
    }

    public Date parse(String input) {
        final int day = parseInt(dayRange.select(input), DEFAULT_RADIX);
        final int month = parseInt(monthRange.select(input), DEFAULT_RADIX);
        final int year = parseInt(yearRange.select(input), DEFAULT_RADIX);
        return Date.newBuilder()
                .setDay(day)
                .setMonth(month)
                .setYear(year)
                .setWeekday(dayOfWeek(day, month, year))
                .build();
    }

    private static int dayOfWeek(int day, int month, int year) {
        int ix = (((year % 100) + 100 - 21) % 28) + monthOffset(month)
                + ((month > 2) ? 1 : 0);
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

    private static class StringRange {

        private final int from;
        private final int to;

        public StringRange(int from, int to) {
            this.from = from;
            this.to = to;
        }

        public String select(String text) {
            if (text.length() < this.to + 1) {
                throw new IllegalArgumentException("Cannot select range " +
                        this + "on '" + text + "'");
            }
            return text.substring(this.from, this.to + 1);
        }

        @Override
        public String toString() {
            return String.format("[%d, %d]", this.from, this.to);
        }
    }
}
