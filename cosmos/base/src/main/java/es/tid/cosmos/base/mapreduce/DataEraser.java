package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
public abstract class DataEraser {
    private static final Map<Class, Class<? extends DataEraser>> erasers;

    static {
        erasers = new HashMap<Class, Class<? extends DataEraser>>();
        erasers.put(FileInputFormat.class, FileDataEraser.class);
        erasers.put(FileOutputFormat.class, FileDataEraser.class);
    }

    public static DataEraser getEraser(Class c) {
        if (!erasers.containsKey(c)) {
            Class superClass = c.getSuperclass();
            return superClass == null ? null : getEraser(superClass);
        }
        try {
            return erasers.get(c).newInstance();
        } catch (InstantiationException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        } catch (IllegalAccessException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        }
    }

    public abstract void deleteInputs(Job job) throws IOException;

    public abstract void deleteOutput(Job job) throws IOException;
}
