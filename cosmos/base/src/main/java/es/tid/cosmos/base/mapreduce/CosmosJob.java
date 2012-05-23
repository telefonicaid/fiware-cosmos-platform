package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author ximo
 */
public abstract class CosmosJob extends Job {
    private volatile ExceptionedThread submittedThread;
    private boolean deleteOutputOnExit;
    private JobList dependencies;

    public CosmosJob(Configuration conf, String jobName)
            throws IOException {
        super(conf, jobName);
        this.submittedThread = null;
        this.deleteOutputOnExit = false;
        this.dependencies = new JobList();
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
                    if (arg instanceof Class) {
                        retVal.add((Class)arg);
                    } else if (arg instanceof ParameterizedType) {
                        retVal.add((Class)((ParameterizedType)arg).getRawType());
                    } else {
                        throw new Exception("Unknown arg type");
                    }
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
    public synchronized final boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        this.submit(verbose);

        this.submittedThread.join();
        this.submittedThread.throwErrors();
        if(!this.callSuperWaitForCompletion(verbose)) {
            throw new JobExecutionException("Job failed: " + this.getJobName());
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

    private static abstract class ExceptionedThread extends Thread {
        private Exception exception;

        protected void setException(Exception e) {
            this.exception = e;
        }

        public void throwErrors() throws IOException, InterruptedException,
                                         ClassNotFoundException {
            if (this.exception == null) {
                return;
            }

            if (this.exception instanceof RuntimeException) {
                throw (RuntimeException)this.exception;
            } else if (this.exception instanceof IOException) {
                throw (IOException)this.exception;
            } else if (this.exception instanceof InterruptedException) {
                throw (InterruptedException)this.exception;
            } else if (this.exception instanceof ClassNotFoundException) {
                throw (ClassNotFoundException)this.exception;
            } else {
                throw new RuntimeException("Unexpected exception thrown",
                                           this.exception);
            }
        }
    }

    @Override
    public void submit() {
        this.submit(true);
    }

    public synchronized void submit(final boolean verbose) {
        if (this.submittedThread != null) {
            return;
        }

        // Create thread for current job
        this.submittedThread = new ExceptionedThread() {
            @Override
            public void run() {
                try {
                    CosmosJob.this.dependencies.waitForCompletion(verbose);
                    CosmosJob.this.callSuperSubmit();
                } catch (Exception ex) {
                    this.setException(ex);
                }
            }
        };

        this.submittedThread.start();
    }

    /**
     * This is a test hook. Please do not override unless you are a test class
     */
    protected void callSuperSubmit() throws IOException, InterruptedException,
                                            ClassNotFoundException {
        super.submit();
    }

    /**
     * This is a test hook. Please do not override unless you are a test class
     */
    protected boolean callSuperWaitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        return super.waitForCompletion(verbose);
    }

    public void addDependentJob(CosmosJob job) {
        if (this.submittedThread != null) {
            throw new IllegalStateException("Cannot add a dependent job to a"
                    + "job if that job has been already submitted");
        }
        if (job.equals(this)) {
            throw new IllegalArgumentException("Cannot add a job to its own"
                    + "dependent job list.");
        }
        this.dependencies.add(job);
    }
}
