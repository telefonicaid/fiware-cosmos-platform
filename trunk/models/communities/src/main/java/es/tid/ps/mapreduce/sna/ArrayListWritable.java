package es.tid.ps.mapreduce.sna;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.EOFException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.apache.hadoop.io.WritableComparable;

/**
 * 
 * ArrayListWritable is a class that implements a list of String elements that
 * will be used like a Key/Values objects. This class is a wrapper of @see
 * java.util.ArrayList <String>
 * 
 * This class serialize all the data like a UTF string, so each element of the
 * array could be read independently. Probably this implementation is not the
 * best and use more bytes that there are necessary for storage all the data.
 * 
 * @author rgc@tid.es
 */
public class ArrayListWritable implements WritableComparable<ArrayListWritable> {

    private List<String> values;

    /**
     * Constructor
     */
    public ArrayListWritable() {
        values = new ArrayList<String>();
    }

    /**
     * Constructor makes a copy of the data from a list
     * 
     * @param values
     */
    public ArrayListWritable(List<String> values) {
        this.values = new ArrayList<String>(values);
    }

    /**
     * Constructor makes a copy of the data from a array
     * 
     * @param array
     */
    public ArrayListWritable(String[] array) {
        values = new ArrayList<String>(Arrays.asList(array));
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof ArrayListWritable)) {
            return false;
        }
        return this.values.equals(((ArrayListWritable) o).getList());
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        for (String value : values) {
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
        values = new ArrayList<String>();
        try {
            while (values.add(in.readUTF())) {
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
    public int compareTo(ArrayListWritable o) {
        // TODO(rgc) Auto-generated method stub
        return 0;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "ArrayListWritable [values=" + values + "]";
    }

    /**
     * Wrapper of @see java.util.ArrayList#add(java.lang.Object)
     * 
     * @param string
     *            object to add
     */
    public void add(String string) {
        values.add(string);
    }

    /**
     * Wrapper of @see java.util.ArrayList#iterator()
     * 
     * @return a iterator over the data Strings
     */
    public Iterator<String> iterator() {
        return values.iterator();
    }

    /**
     * Method that return the ArrayList
     * 
     * @return the data of the ArrayList
     */
    public List<String> getList() {
        return values;
    }
}
