package es.tid.analytics.mobility.core.data;

import org.apache.hadoop.io.WritableComparable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Calendar;
import java.util.Date;

public class GLEvent implements WritableComparable<GLEvent> {
    private long userId;
    private long placeId;
    private Date date;

    public long getUserId() {
        return userId;
    }

    public void setUserId(final long idNode) {
        this.userId = idNode;
    }

    public long getPlaceId() {
        return placeId;
    }

    public void setPlaceId(final long idLocation) {
        this.placeId = idLocation;
    }

    public Date getDate() {
        return date;
    }

    public void setDate(final Date date) {
        this.date = date;
    }

    public GLEvent() {
    }

    public GLEvent(final long idNode, final long idLocation, final Date date) {
        this.userId = idNode;
        this.placeId = idLocation;
        this.date = date;

    }

    @SuppressWarnings("deprecation")
    public byte getHour() {
        int aux = -1;
        if (this.date != null) {
            // getHours() return 0-23
            aux = this.date.getHours();

            return (byte) aux;
        }

        return (byte) aux;
    }

    @SuppressWarnings("deprecation")
    public byte getWeekDay() {
        byte weekday = -1;

        if (this.date != null) {
            // getDay() return 0-6
            // return values SUNDAY, MONDAY, TUESDAY,
            // WEDNESDAY, THURSDAY, FRIDAY, and SATURDAY (from 1 to 7)
            weekday = (byte) (this.date.getDay() + 1);

        }
        return weekday;

    }

    public byte getHour(final Calendar dateCalendar) {
        if (dateCalendar != null) {
            // HOUR_OF_DAY is used for the 24-hour clock
            return (byte) dateCalendar.get(Calendar.HOUR_OF_DAY);
        }

        return (byte) -1;
    }

    public byte getWeekDay(final Calendar dateCalendar) {
        byte weekday = -1;

        if (dateCalendar != null) {
            // DAY_OF_WEEK: This field takes values SUNDAY, MONDAY, TUESDAY,
            // WEDNESDAY, THURSDAY, FRIDAY, and SATURDAY (from 1 to 7)
            weekday = (byte) dateCalendar.get(Calendar.DAY_OF_WEEK);
        }
        return weekday;

    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((date == null) ? 0 : date.hashCode());
        result = prime * result + (int) (placeId ^ (placeId >>> 32));
        result = prime * result + (int) (userId ^ (userId >>> 32));
        return result;
    }

    @Override
    public boolean equals(final Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final GLEvent other = (GLEvent) obj;
        if (date == null) {
            if (other.date != null) {
                return false;
            }
        } else if (!date.equals(other.date)) {
            return false;
        }
        if (placeId != other.placeId) {
            return false;
        }
        if (userId != other.userId) {
            return false;
        }
        return true;
    }

    @Override
    public void write(final DataOutput dataOutput) throws IOException {
        dataOutput.writeLong(this.userId);
        dataOutput.writeLong(this.placeId);
        dataOutput.writeLong(this.date.getTime());
    }

    @Override
    public void readFields(final DataInput dataInput) throws IOException {
        this.userId = dataInput.readLong();
        this.placeId = dataInput.readLong();
        this.date = new Date(dataInput.readLong());
    }

    @Override
    public int compareTo(final GLEvent event) {
        if (event.getUserId() > this.getUserId()) {
            return 1;
        } else if (event.getUserId() < this.getUserId()) {
            return -1;
        } else {
            if (event.getPlaceId() > this.getPlaceId()) {
                return 1;
            } else if (event.getPlaceId() < this.getPlaceId()) {
                return -1;
            } else {
                return event.getDate().compareTo(this.getDate());
            }
        }
    }

    @Override
    public String toString() {
        return "GLEvent{" + "idNode=" + userId + ", idLocation=" + placeId + ", date=" + date + '}';
    }
}