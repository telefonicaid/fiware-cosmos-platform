package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author ximo
 */
public abstract class CosmosJob extends Job implements Runnable {
    public CosmosJob(Configuration conf, String jobName)
            throws IOException {
        super(conf, jobName);
    }

    /**
     * This method returns the list of types used for the closest generic type
     * implemented by "originalClass". For example, if A extends B&lt;T&gt; and
     * B&lt;T&gt; extends C&lt;String,T&gt;, then calling this method with
     * A.class will return an array that contains a single element: T.class
     *
     * @param originalClass
     * @return the list of types used for the closest generic type implemented
     * by "originalClass"
     * @throws Exception
     */
    protected static Class[] getGenericParameters(Class originalClass)
            throws Exception {
        try {
            Type parent = originalClass.getGenericSuperclass();
            Class superClass;
            if (parent instanceof ParameterizedType) {
                ParameterizedType parameterizedType = (ParameterizedType)parent;
                Type[] args = parameterizedType.getActualTypeArguments();
                ArrayList<Class> retVal = new ArrayList<Class>(args.length);
                for (Type arg : args) {
                    retVal.add((Class)arg);
                }
                return retVal.toArray(new Class[0]);
            } else {
                if (!(parent instanceof Class)) {
                    throw new Exception("parent is not a class! parent: "
                            + parent.toString());
                }
                superClass = (Class)parent;
                if (superClass == Object.class) {
                    throw new Exception("The passed in type does not extend any"
                            + " generic class!");
                }
            }
            return getGenericParameters(superClass);
        } catch (Exception ex) {
            throw new Exception("[Debug Info] originalClass: "
                    + originalClass.toString(), ex);
        }
    }

    @Override
    public final void waitForCompletion(EnumSet<CleanupOptions> options)
            throws Exception {
        if (!this.waitForCompletion(true)) {
            throw new Exception("Failed to run " + this.getJobName());
        }
        if (options.contains(CleanupOptions.DeleteInput)) {
            Class inputFormat = this.getInputFormatClass();
            DataEraser eraser = DataEraser.getEraser(inputFormat);
            if (eraser == null) {
                throw new UnsupportedOperationException("CosmosJob is not prepared"
                        + " to handle deleting inputs of type "
                        + inputFormat.getSimpleName() + ". Please implement a new "
                        + "DataEraser that handles this case.");
            }
            eraser.deleteInputs(this);
        }
        if (options.contains(CleanupOptions.DeleteOutput)) {
            Class outputFormat = this.getOutputFormatClass();
            DataEraser eraser = DataEraser.getEraser(outputFormat);
            if (eraser == null) {
                throw new UnsupportedOperationException("CosmosJob is not prepared"
                        + " to handle deleting outputs of type "
                        + outputFormat.getSimpleName() + ". Please implement a new "
                        + "DataEraser that handles this case.");
            }
            eraser.deleteOutput(this);
        }
    }

    @Override
    public final List<CosmosJob> getJobs() {
        List<CosmosJob> ret = new ArrayList<CosmosJob>();
        ret.add(this);
        return ret;
    }
}