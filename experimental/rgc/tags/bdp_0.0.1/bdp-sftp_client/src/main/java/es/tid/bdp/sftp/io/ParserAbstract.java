package es.tid.bdp.sftp.io;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.utils.PropertiesPlaceHolder;

public abstract class ParserAbstract {

    private static final String INPUT_ATTRS_REGEX = "input.attrs.regex";
    private static final String INPUT_ATTRS_DESC = "input.attrs.desc";

    protected Pattern pattern;
    protected Map<String, Integer> regPosition;

    public ParserAbstract() throws IOException {

        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        this.pattern = Pattern.compile(properties
                .getProperty(INPUT_ATTRS_REGEX));


        // String lineConfigurator =
        // "Cdr.userId|Cdr.cellId|Cdr.date|Date.weekday|Date.year|Date.month|Date.day|Cdr.time|Time.hour|Time.minute|Time.seconds";

        // this.pattern = Pattern
        // .compile("(.+)\\|(.+)\\|((.+)\\|(\\d{2})/(\\d{2})/(\\d{4}))\\|((\\d{2}):(\\d{2}):(\\d{2}))");

        this.regPosition = new HashMap<String, Integer>();

        String[] array = properties.getProperty(INPUT_ATTRS_DESC).split("\\|");

        for (int i = 0; i < array.length; i++) {
            this.regPosition.put(array[i], i + 1);
        }
    }

    public abstract ProtobufWritable<Message> parseLine(String cdrLine);

}
