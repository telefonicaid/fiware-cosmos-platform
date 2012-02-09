package es.tid.mapreduce.mobility2.data;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Calendar;

import org.apache.hadoop.io.Writable;

/**
 * Pojo that storage the data that are sent from the mapper to the reduces, this
 * cojo storage the bts in where was done the call and the date
 * 
 * @author rgc
 */
public class UserMobilityData implements Writable {
    private String bts;
    private Calendar dateCall;

    public UserMobilityData() {
    }

    public UserMobilityData(String bts, Calendar dateCall) {
        super();
        this.bts = bts;
        this.dateCall = dateCall;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(bts);
        out.writeLong(dateCall.getTimeInMillis());
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#readFields(java.io.DataInput)
     */
    @Override
    public void readFields(DataInput in) throws IOException {
        dateCall = Calendar.getInstance();
        bts = in.readUTF();
        dateCall.setTimeInMillis(in.readLong());
    }

    /**
     * @return the bts
     */
    public String getBts() {
        return bts;
    }

    /**
     * @param bts
     *            the bts to set
     */
    public void setBts(String bts) {
        this.bts = bts;
    }

    /**
     * @return the dateCall
     */
    public Calendar getDateCall() {
        return dateCall;
    }

    /**
     * @param dateCall
     *            the dateCall to set
     */
    public void setDateCall(Calendar dateCall) {
        this.dateCall = dateCall;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((bts == null) ? 0 : bts.hashCode());
        result = prime * result
                + ((dateCall == null) ? 0 : dateCall.hashCode());
        return result;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        UserMobilityData other = (UserMobilityData) obj;
        if (bts == null) {
            if (other.bts != null)
                return false;
        } else if (!bts.equals(other.bts))
            return false;
        if (dateCall == null) {
            if (other.dateCall != null)
                return false;
        } else if (!dateCall.equals(other.dateCall))
            return false;
        return true;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "UserMobilityData [bts=" + bts + ", dateCall=" + dateCall + "]";
    }
}
