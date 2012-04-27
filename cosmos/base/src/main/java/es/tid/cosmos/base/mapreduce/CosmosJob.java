package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author ximo
 */
public abstract class CosmosJob extends Job {
    private boolean submitted;
    private boolean deleteOutputOnExit;
    private List<CosmosJob> dependencies;

    public CosmosJob(Configuration conf, String jobName)
            throws IOException {
        super(conf, jobName);
        this.submitted = false;
        this.deleteOutputOnExit = false;
        this.dependencies = new LinkedList<CosmosJob>();
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

    /**
     * This method submits and waits for this job and all its dependent jobs. It
     * signals errors through JobExecutionException, so the return value will
     * never be false.
     *
     * @param verbose
     * @return Always true
     * @throws IOException
     * @throws InterruptedException
     * @throws ClassNotFoundException
     * @throws JobExecutionException
     */
    @Override
    public final boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        for (CosmosJob dependency : this.dependencies) {
            dependency.internalSubmit();
        }
        for (CosmosJob dependency : this.dependencies) {
            dependency.waitForCompletion(verbose);
        }
        if (!super.waitForCompletion(verbose)) {
            throw new JobExecutionException("Failed to run " + this.getJobName());
        }

        if (this.deleteOutputOnExit) {
            Class outputFormat = this.getOutputFormatClass();
            OutputEraser eraser = OutputEraser.getEraser(outputFormat);
            if (eraser == null) {
                throw new UnsupportedOperationException("CosmosJob is not prepared"
                        + " to handle deleting outputs of type "
                        + outputFormat.getSimpleName() + ". Please implement a new "
                        + "DataEraser that handles this case.");
            }
            eraser.deleteOutput(this);
        }

        return true;
    }

    public void setDeleteOutputOnExit(boolean deleteOutputOnExit) {
        this.deleteOutputOnExit = deleteOutputOnExit;
    }

    public boolean getDeleteOutputOnExit() {
        return this.deleteOutputOnExit;
    }

    @Override
    public void submit() throws IOException, InterruptedException,
                                ClassNotFoundException {
        if (!this.dependencies.isEmpty()) {
            throw new IllegalStateException("Cannot submit a job that has"
                    + " dependencies. Please use waitForCompletion");
        }
        super.submit();
        this.submitted = true;
    }

    private void internalSubmit() throws IOException, InterruptedException,
                                         ClassNotFoundException {
        if (!this.submitted) {
            for (CosmosJob dependency : this.dependencies) {
                dependency.internalSubmit();
            }
            super.submit();
            this.submitted = true;
        }
    }

    public void addDependentJob(CosmosJob job) {
        this.dependencies.add(job);
    }
}