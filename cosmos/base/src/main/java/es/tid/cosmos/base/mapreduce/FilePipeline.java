package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 *
 * @author ximo
 */
public class FilePipeline implements Runnable {
    private LinkedList<CosmosJob> jobs;

    public static FilePipeline merge(Runnable[] inputs, Runnable runnable)
            throws IOException, ClassNotFoundException {
        FilePipeline mergedPipe = new FilePipeline();
        ArrayList<Path> inputPaths = new ArrayList<Path>(inputs.length);
        for (Runnable inputRunnable : inputs) {
            List<CosmosJob> jobs = inputRunnable.getJobs();
            // Verify jobs have the correct types
            int index = 0;
            for (CosmosJob job : jobs) {
                if (index != 0 && !FileInputFormat.class.isAssignableFrom(
                        job.getInputFormatClass())) {
                    throw new InvalidParameterException("One of the input jobs does"
                            + " not use a FileInputFormat as the intermediate"
                            + " state");
                }

                if (!FileOutputFormat.class.isAssignableFrom(
                        job.getOutputFormatClass())) {
                    throw new InvalidParameterException("One of the input jobs does"
                            + " not use a FileOutputFormat as the intermediate"
                            + " state");
                }
            }
            Path outputPath = FileOutputFormat.getOutputPath(
                    jobs.get(jobs.size() - 1));
            if (outputPath == null) {
                throw new InvalidParameterException(
                        "inputs must be fully configured but at least one of "
                        + "them doesn't have an output path set");
            }
            inputPaths.add(outputPath);
            mergedPipe.jobs.addAll(jobs);
        }

        List<CosmosJob> jobs = runnable.getJobs();
        // Verify jobs have the correct types
        int index = 0;
        final int jobsCount = jobs.size();
        for (CosmosJob job : jobs) {
            if (!FileInputFormat.class.isAssignableFrom(
                    job.getInputFormatClass())) {
                throw new InvalidParameterException("One of the runnable jobs does"
                        + " not use a FileInputFormat as the intermediate"
                        + " state");
            }

            if (index != jobsCount - 1 && !FileOutputFormat.class.isAssignableFrom(
                    job.getOutputFormatClass())) {
                throw new InvalidParameterException("One of the runnable jobs does"
                        + " not use a FileOutputFormat as the intermediate"
                        + " state");
            }
        }

        FileInputFormat.setInputPaths(jobs.get(jobs.size() - 1),
                                      inputPaths.toArray(new Path[0]));
        mergedPipe.jobs.addAll(jobs);
        return mergedPipe;
    }

    private void signalBadConstructionParams() {
        throw new InvalidParameterException("This constructor requires an"
                + "odd number of parameters: [Job, (Path, Job)*]");
    }

    /**
     * This constructor will configure the passed in Jobs with the passed in
     * Paths. It expects and odd number of arguments that matches the following
     * patters: [Runnable, (Path, Runnable)*].
     *
     * Example: given the arguments (job1, path1, job2, path2, job3) the jobs
     * will be configured as follows: - job1's input will not be configured -
     * job1's output will be path1 - job2's input will be path1 - job2's output
     * will be path2 - job3's input will be path2 - job3's output will not be
     * configured
     *
     * @param args
     * @throws IOException
     */
    public FilePipeline(Runnable first, Object... args) throws IOException {
        this();
        if (args.length % 2 != 0) {
            this.signalBadConstructionParams();
        }

        this.jobs.addAll(first.getJobs());

        // Verify parameters
        for (int i = 0; i < args.length; i += 2) {
            if (!(args[i] instanceof Path)) {
                this.signalBadConstructionParams();
            }
            if (!(args[i + 1] instanceof Runnable)) {
                this.signalBadConstructionParams();
            }
        }

        // Configure jobs
        for (int i = 0; i < args.length; i += 2) {
            Path path = (Path)args[i];
            FileOutputFormat.setOutputPath(this.jobs.getLast(), path);
            Runnable runnable = (Runnable)args[i + 1];
            List<CosmosJob> runnableJobs = runnable.getJobs();
            FileInputFormat.setInputPaths(runnableJobs.get(0), path);
            this.jobs.addAll(runnableJobs);
        }
    }

    private FilePipeline() {
        this.jobs = new LinkedList<CosmosJob>();
    }

    @Override
    public void waitForCompletion(EnumSet<CleanupOptions> options)
            throws Exception {
        int processedJobsCount = 0;


        try {
            for (CosmosJob job : this.jobs) {
                job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
                processedJobsCount++;
            }
        } finally {
            if (options.contains(CleanupOptions.DeleteIntermediateResults)) {
                // Don't delete the last job's output
                List<CosmosJob> processedJobs = this.jobs.subList(
                        0,
                        Math.min(processedJobsCount, this.jobs.size() - 1));
                if (!processedJobs.isEmpty()) {
                    // All intermediate results have the same type
                    DataEraser eraser = DataEraser.getEraser(
                            processedJobs.get(0).getOutputFormatClass());
                    for (CosmosJob job : processedJobs) {
                        eraser.deleteOutput(job);
                    }
                }
            }
            if (options.contains(CleanupOptions.DeleteInput)) {
                CosmosJob firstJob = this.jobs.getFirst();
                DataEraser.getEraser(
                        firstJob.getInputFormatClass()).deleteInputs(firstJob);
            }
            if (options.contains(CleanupOptions.DeleteOutput)) {
                CosmosJob lastJob = this.jobs.getLast();
                DataEraser.getEraser(
                        lastJob.getOutputFormatClass()).deleteOutput(lastJob);
            }
        }
    }

    @Override
    public List<CosmosJob> getJobs() {
        return new ArrayList<CosmosJob>(this.jobs);
    }
}