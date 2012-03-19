package es.tid.bdp.utils.parse;

import java.util.regex.Matcher;

import com.google.protobuf.Message;

import es.tid.ps.kpicalculation.data.WebLogPB.WebLog;

public class ParserWebLog extends ParserAbstract {

    private final String ATTR_VISITOR_ID = "visitorId";
    private final String ATTR_FULL_URL = "fullUrl";
    private final String ATTR_DATE = "date";
    private final String ATTR_HTTP_STATUS = "httpStatus";
    private final String ATTR_MIME_TYPE = "mimeType";
    private final String ATTR_USER_AGENT = "userAgent";
    private final String ATTR_METHOD = "method";

    public ParserWebLog(String pattern, String attr) {
        super(pattern, attr);
    }

    @Override
    protected Message createMessage() {

        WebLog.Builder builder = WebLog.newBuilder();

        builder.setVisitorId(matcher.group(regPosition.get(ATTR_VISITOR_ID)));
        builder.setFullUrl(matcher.group(regPosition.get(ATTR_FULL_URL)));
        builder.setDate(matcher.group(regPosition.get(ATTR_DATE)));
        builder.setHttpStatus(matcher.group(regPosition.get(ATTR_HTTP_STATUS)));
        builder.setMimeType(matcher.group(regPosition.get(ATTR_MIME_TYPE)));
        builder.setUserAgent(matcher.group(regPosition.get(ATTR_USER_AGENT)));
        builder.setMethod(matcher.group(regPosition.get(ATTR_METHOD)));

        return builder.build();
    }

}
