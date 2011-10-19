package es.tid.ps.mapreduce.sna;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.EOFException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.hadoop.io.WritableComparable;

/**
 * This class is used to storage the values generate in the map process for
 * generating the SNA. It implements writable because these data will be storage
 * into hadoop file system and comparable because we need compared this type of
 * class for unit testing.
 * 
 * @author rgc
 * 
 */
public class NodeCombination implements WritableComparable<NodeCombination> {
    // Storage the central user for calculate the sna Algorithm
    private String principal;
    // Storage the rest of the users with the principal has had a call
    private List<String> values;

    /**
     * Constructor
     */
    public NodeCombination() {}

    /**
     * Constructor
     * 
     * @param principal
     *            copy the value into principal attribute
     * @param values
     *            make a pointer from values to this object
     */
    public NodeCombination(String principal, List<String> values) {
        this.principal = principal;
        this.values = values;
    }

    /**
     * Constructor
     * 
     * @param principal
     *            copy the value into principal attribute
     * @param array
     *            generate a new arraylist into value and load it with the
     *            values of this param
     */
    public NodeCombination(String principal, String[] array) {
        this.values = new ArrayList<String>(Arrays.asList(array));
        this.principal = principal;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.principal);
        for (String value : this.values) {
            out.writeUTF(value);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#readFields(java.io.DataInput)
     */
    @Override
    public void readFields(DataInput in) throws IOException {
        this.principal = in.readUTF();
        this.values = new ArrayList<String>();
        try {
            while (this.values.add(in.readUTF())) {
            }
        } catch (EOFException e) {
            // When catch this exception it is because all parameter are reading
            // and it arrives at the end of the structure, so it is no necesary
            // to do anything because catching this exception is the normal way
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     */
    @Override
    public int compareTo(NodeCombination o) {
        return this.principal.compareTo(o.principal);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof NodeCombination)) {
            return false;
        }
        return this.principal.equals(((NodeCombination) o).principal);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return ("Numbers [principal=" + this.principal + ", values=" + this.values + "]");
    }

    /**
     * @param principal
     *            the principal to set
     */
    public void setPrincipal(String principal) {
        this.principal = principal;
    }

    /**
     * @return the principal
     */
    public String getPrincipal() {
        return this.principal;
    }

    /**
     * @param values
     *            the values to set
     */
    public void setValues(List<String> values) {
        this.values = values;
    }

    /**
     * @return the values
     */
    public List<String> getValues() {
        return this.values;
    }
}
