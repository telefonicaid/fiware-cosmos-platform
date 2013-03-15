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

package es.tid.cosmos.platform.injection.flume;

import java.util.ArrayList;
import java.util.List;

import org.apache.flume.Event;
import org.apache.flume.interceptor.Interceptor;

/**
 * This abstract class implements the common behaviour of any Flume
 * interceptor.
 *
 * @author apv
 */
public abstract class AbstractInterceptor implements Interceptor {

    /**
     * Default implementation for initialization of interceptor objects. By
     * default, there is no specific behavior associated to interceptor
     * initialization. If needed, concrete interceptors may override this
     * method.
     */
    @Override
    public void initialize() {
    }

    @Override
    public List<Event> intercept(List<Event> events) {
        List<Event> result = new ArrayList<Event>(events.size());
        for (Event inputEvent : events) {
            Event outputEvent = intercept(inputEvent);
            if (outputEvent != null) {
                result.add(outputEvent);
            }
        }
        return result;
    }

    /**
     * Default implementation for closing of interceptor objects. By
     * default, there is no specific behavior associated to interceptor
     * closing. If needed, concrete interceptors may override this
     * method.
     */
    @Override
    public void close() {
    }
}
