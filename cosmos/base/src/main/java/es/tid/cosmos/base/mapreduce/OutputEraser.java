package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.OutputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
abstract class OutputEraser {
    private static final Map<Class<? extends OutputFormat>,
                             Class<? extends OutputEraser>> ERASERS =
        new HashMap() {{
            put(FileOutputFormat.class, FileDataEraser.class);
        }};

    public static OutputEraser getEraser(Class c) {
        if (!ERASERS.containsKey(c)) {
            Class superClass = c.getSuperclass();
            return superClass == null ? null : getEraser(superClass);
        }
        try {
            return ERASERS.get(c).newInstance();
        } catch (InstantiationException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        } catch (IllegalAccessException ex) {
            throw new IllegalArgumentException(
                    "Could not create instance of eraser class", ex);
        }
    }

    public abstract void deleteOutput(Job job) throws IOException;
}
