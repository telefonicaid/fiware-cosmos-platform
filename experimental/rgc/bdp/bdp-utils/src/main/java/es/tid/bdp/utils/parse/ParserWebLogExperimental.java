package es.tid.bdp.utils.parse;

import java.util.StringTokenizer;
import java.util.regex.Matcher;

import com.google.protobuf.Message;

import es.tid.ps.kpicalculation.data.WebLogPB.WebLog;

public class ParserWebLogExperimental extends ParserAbstract {

    public ParserWebLogExperimental(String pattern, String attr) {
        super(pattern, attr);
        // TODO Auto-generated constructor stub
    }
    
    @Override
    public Message parseLine(final String line) {
        StringTokenizer st = new StringTokenizer(line, "\t");
       
        WebLog.Builder builder = WebLog.newBuilder();

        builder.setVisitorId(st.nextToken());
        builder.setFullUrl(st.nextToken());
        builder.setDate(st.nextToken());
        builder.setHttpStatus(st.nextToken());
        builder.setMimeType(st.nextToken());
        builder.setUserAgent(st.nextToken());
        builder.setMethod(st.nextToken());
        return builder.build();

        
    }

    @Override
    protected Message createMessage(Matcher matcher) {
        // TODO Auto-generated method stub
        return null;
    }

}
