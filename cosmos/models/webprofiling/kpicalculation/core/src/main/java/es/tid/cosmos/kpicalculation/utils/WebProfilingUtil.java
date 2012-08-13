/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.kpicalculation.utils;

import java.util.StringTokenizer;

import org.apache.nutch.util.URLUtil;
import org.apache.xerces.util.URI;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;
import es.tid.cosmos.kpicalculation.data.KpiCalculationDataException;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;

/**
 * Class used to serialize records from text files
 *
 * @author javierb, sortega
 */
public final class WebProfilingUtil {
    private static final String DELIMITER = "\t";

    private static WebProfilingLog.Builder builder;
    private static KpiCalculationDateFormatter dateFormatter;

    private WebProfilingUtil() {
        // Utility class
    }

    public static WebProfilingLog getInstance(String line) {
        if (builder == null) {
            init();
        }
        return set(line);
    }

    private static void init() {
        dateFormatter = new KpiCalculationDateFormatter();
        builder = WebProfilingLog.newBuilder();
    }

    private static WebProfilingLog set(String line) {
        try {
            StringTokenizer stt = new StringTokenizer(line,
                    WebProfilingUtil.DELIMITER);
            builder = WebProfilingLog.newBuilder();
            builder.setVisitorId(stt.nextToken());
            builder.setFullUrl(stt.nextToken());

            URI uri = new URI(builder.getFullUrl());
            builder.setProtocol(uri.getScheme());
            builder.setUrlDomain(URLUtil.getDomainName(builder.getFullUrl()));
            builder.setUrlPath(uri.getPath());
            String urlQuery = (uri.getQueryString() != null) ? uri
                    .getQueryString() : "null";
            builder.setUrlQuery(urlQuery);

            builder.setDate(dateFormatter.getValue(stt.nextToken()));
            builder.setStatus(stt.nextToken());
            builder.setMimeType(stt.nextToken());

            String clientInfo = stt.nextToken();
            builder.setUserAgent(clientInfo);
            builder.setBrowser(clientInfo);
            builder.setDevice(clientInfo);
            builder.setOperSys(clientInfo);

            builder.setMethod(stt.nextToken());

            if (stt.hasMoreTokens()) {
                throw new KpiCalculationDataException(
                        "The line has too many fields",
                        KpiCalculationCounter.WRONG_LINE_FORMAT);
            }
            return builder.build();
        } catch (Exception ex) {
            throw new KpiCalculationDataException("The URL is malformed", ex,
                    KpiCalculationCounter.MALFORMED_URL);
        }
    }
}
