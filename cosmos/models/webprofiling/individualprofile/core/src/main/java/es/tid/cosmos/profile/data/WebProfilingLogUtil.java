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

package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Construction utils for WebProfilingLog class.
 *
 * @author dmicol
 */
public final class WebProfilingLogUtil {
    private WebProfilingLogUtil() {
    }

    public static WebProfilingLog create(String visitorId, String protocol,
                                         String fullUrl, String urlDomain,
                                         String urlPath, String urlQuery,
                                         String date, String userAgent,
                                         String browser, String device,
                                         String operSys, String method,
                                         String status, String mimeType) {
        return WebProfilingLog.newBuilder()
                .setVisitorId(visitorId)
                .setProtocol(protocol)
                .setFullUrl(fullUrl)
                .setUrlDomain(urlDomain)
                .setUrlPath(urlPath)
                .setUrlQuery(urlQuery)
                .setDate(date)
                .setUserAgent(userAgent)
                .setBrowser(browser)
                .setDevice(device)
                .setOperSys(operSys)
                .setMethod(method)
                .setStatus(status)
                .setMimeType(mimeType)
                .build();
    }

    public static ProtobufWritable<WebProfilingLog> createAndWrap(
            String visitorId, String protocol, String fullUrl, String urlDomain,
            String urlPath, String urlQuery, String date, String userAgent,
            String browser, String device, String operSys, String method,
            String status, String mimeType) {
        ProtobufWritable<WebProfilingLog> wrapper =
                ProtobufWritable.newInstance(WebProfilingLog.class);
        wrapper.set(create(visitorId, protocol, fullUrl, urlDomain, urlPath,
                           urlQuery, date, userAgent, browser, device, operSys,
                           method, status, mimeType));
        return wrapper;
    }
}
