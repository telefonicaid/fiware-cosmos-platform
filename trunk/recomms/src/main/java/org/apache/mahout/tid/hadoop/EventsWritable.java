package org.apache.mahout.tid.hadoop;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.mahout.tid.hadoop.EventsWritable;
import org.apache.mahout.common.RandomUtils;
import org.apache.mahout.math.VarLongWritable;

/**
 * A {@link org.apache.hadoop.io.Writable} encapsulating an item ID and a
 * preference value.
 */
public final class EventsWritable extends VarLongWritable {
    private float eventValue;
    private long eventType;
    private boolean isboolean;

    public EventsWritable() {
    }

    public EventsWritable(long itemID, long eventType, float eventValue) {
        super(itemID);
        this.eventValue = eventValue;
        this.eventType = eventType;
        this.isboolean = false;
    }

    public EventsWritable(long itemID, long eventType) {
        this(itemID, eventType, 1.0f);
        this.isboolean = true;
    }

    public EventsWritable(EventsWritable other) {
        this(other.get(), other.getEventType(), other.getEventValue());
    }

    public long getID() {
        return get();
    }

    public float getEventValue() {
        return eventValue;
    }

    public long getEventType() {
        return eventType;
    }

    public boolean getisBoolean() {
        return isboolean;
    }

    public void set(long id, float eventValue, long eventType) {
        set(id);
        this.eventValue = eventValue;
        this.eventType = eventType;
    }

    @Override
    public void write(DataOutput out) throws IOException {
        super.write(out);
        out.writeFloat(eventValue);
        out.writeLong(eventType);
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        super.readFields(in);
        eventValue = in.readFloat();
        eventType = in.readLong();
    }

    @Override
    public int hashCode() {
        return super.hashCode() ^ RandomUtils.hashFloat(eventValue);
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof EventsWritable)) {
            return false;
        }
        EventsWritable other = (EventsWritable) o;
        return get() == other.get() && eventValue == other.getEventValue()
                && eventType == other.getEventType();
    }

    @Override
    public String toString() {
        return get() + "\t" + eventValue;
    }

    @Override
    public EventsWritable clone() {
        return new EventsWritable(get(), eventType, eventValue);
    }

}