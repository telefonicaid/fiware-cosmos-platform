package es.tid.bdp.sftp.io;

import java.io.IOException;
import java.util.regex.Matcher;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.profile.data.CdrP.Cdr;
import es.tid.bdp.profile.data.CdrP.Date;
import es.tid.bdp.profile.data.CdrP.Time;

public class ParserCdr extends ParserAbstract {

    public ParserCdr() throws IOException {
        super();
        // TODO Auto-generated constructor stub
    }

    @Override
    public ProtobufWritable<Message> parseLine(String cdrLine) {
        Cdr.Builder cdr = Cdr.newBuilder();
        Matcher m = this.pattern.matcher(cdrLine);

        if (m.matches()) {

            cdr.setCellId(Long.parseLong(m.group(regPosition.get("cellId")), 16));
            try {
                cdr.setUserId(Long.parseLong(
                        m.group(regPosition.get("userId")), 16));
            } catch (NumberFormatException e) {
                cdr.setUserId(0L);
            }
            cdr.setDate(parseDate(m));
            cdr.setTime(parseTime(m));

            ProtobufWritable<Message> protobufWritable = ProtobufWritable
                    .newInstance(Message.class);
            protobufWritable.set(cdr.build());

            return protobufWritable;
        } else {
            throw new RuntimeException("no matches");
        }
    }

    protected Time parseTime(Matcher m) {
        return Time
                .newBuilder()
                .setHour(Integer.parseInt(m.group(regPosition.get("hour"))))
                .setMinute(Integer.parseInt(m.group(regPosition.get("minute"))))
                .setSeconds(
                        Integer.parseInt(m.group(regPosition.get("second"))))
                .build();
    }

    protected Date parseDate(Matcher m) {
        final int day = Integer.parseInt(m.group(regPosition.get("day")));
        final int month = Integer.parseInt(m.group(regPosition.get("month")));
        final int year = Integer.parseInt(m.group(regPosition.get("year")));

        return Date.newBuilder().setDay(day).setMonth(month).setYear(year)
                .setWeekday(dayOfWeek(day, month, year)).build();
    }

    private static int dayOfWeek(int day, int month, int year) {
        year += 100; // real year + 2000 - 1900
        int ix = ((year - 21) % 28) + monthOffset(month)
                + ((month > 2) ? 1 : 0);
        int tx = (ix + (ix / 4)) % 7 + day;
        return (tx + 1) % 7;
    }

    private static int monthOffset(int month) {
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
