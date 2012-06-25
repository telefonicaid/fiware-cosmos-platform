package es.tid.smartsteps.flume;

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

    @Override
    public void initialize() {
        // Nothing to do
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

    @Override
    public void close() {
        // Nothing to do
    }
}
