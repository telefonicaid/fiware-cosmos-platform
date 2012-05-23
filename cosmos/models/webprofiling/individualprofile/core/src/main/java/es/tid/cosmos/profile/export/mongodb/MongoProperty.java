package es.tid.cosmos.profile.export.mongodb;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import com.mongodb.DBObject;
import com.mongodb.hadoop.MongoOutput;
import org.apache.hadoop.io.WritableComparable;

/**
 * Serializable key maps to a given Mongo document property.
 * 
 * Use it instead of Text to map output keys with a property different to 
 * "_id".
 *
 * @author sortega
 */
public class MongoProperty implements MongoOutput,
                                      WritableComparable<MongoProperty> {
    private String property;
    private String value;

    public MongoProperty() {
    }

    public MongoProperty(String property, String value) {
        this.property = property;
        this.value = value;
    }

    public String getProperty() {
        return this.property;
    }

    public void setProperty(String property) {
        this.property = property;
    }

    public String getValue() {
        return this.value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public void appendAsKey(DBObject document) {
        this.appendToDocument(document);
    }

    @Override
    public void appendAsValue(DBObject document) {
        this.appendToDocument(document);
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.property);
        out.writeUTF(this.value);
    }

    @Override
    public void readFields(DataInput input) throws IOException {
        this.property = input.readUTF();
        this.value = input.readUTF();
    }

    @Override
    public int compareTo(MongoProperty other) {
        return this.value.compareTo(other.value);
    }

    private void appendToDocument(DBObject document) {
        document.put(this.property, this.value);
    }
}
