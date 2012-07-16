package es.tid.cosmos.mobility.parsing;

import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author dmicol, sortega
 */
public abstract class Parser {

    protected static final int MAX_CELL_DIGITS = 8;
    protected static final int MAX_CLIENT_LENGTH = 10;
    protected static final int DEFAULT_RADIX = 10;

    protected final String line;
    private final String[] values;
    private int index;

    public Parser(String line, String separator) {
        this.line = line;
        StringTokenizer tokenizer = new StringTokenizer(line, separator, true);
        List<String> valuesList = new LinkedList<String>();
        boolean isPrecededBySeparator = true;
        boolean isSeparator;
        while (tokenizer.hasMoreTokens()) {
            final String token = tokenizer.nextToken();
            isSeparator = token.equals(separator);
            if (isSeparator) {
                if (isPrecededBySeparator) {
                    valuesList.add("");
                }
            } else {
                valuesList.add(token);
            }
            isPrecededBySeparator = isSeparator;
        }
        this.values = valuesList.toArray(new String[valuesList.size()]);
        this.index = 0;
    }

    public abstract Object parse();

    protected String nextToken() {
        return this.values[this.index++];
    }

    protected int parseInt() {
        return Integer.parseInt(this.nextToken(), DEFAULT_RADIX);
    }

    protected long parseLong() {
        return Long.parseLong(this.nextToken(), DEFAULT_RADIX);
    }

    protected double parseDouble() {
        return Double.parseDouble(this.nextToken());
    }

    protected long parseCellId() {
        String str = this.nextToken();
        if (str.isEmpty()) {
            return 0L;
        } else if (str.length() > MAX_CELL_DIGITS) {
            // Take the least significant hex digits
            str = str.substring(str.length() - MAX_CELL_DIGITS);
        }
        return Long.parseLong(str, 16);
    }

    protected long parseUserId() {
        String str = this.nextToken();
        if (str.length() > MAX_CLIENT_LENGTH) {
            str = str.substring(str.length() - MAX_CLIENT_LENGTH);
        }
        return Long.parseLong(str, DEFAULT_RADIX);
    }

    protected ClusterVector parseClusterVector() {
        ClusterVector.Builder clusterVector = ClusterVector.newBuilder();
        for (int i = 0; i < 96; i++) {
            clusterVector.addComs(this.parseDouble());
        }
        return clusterVector.build();
    }

    protected Time parseTime() {
        String time = this.nextToken();
        return Time.newBuilder()
                .setHour(Integer.parseInt(time.substring(0, 2), DEFAULT_RADIX))
                .setMinute(Integer.parseInt(time.substring(3, 5), DEFAULT_RADIX))
                .setSeconds(Integer.parseInt(time.substring(6, 8), DEFAULT_RADIX))
                .build();
    }
}
