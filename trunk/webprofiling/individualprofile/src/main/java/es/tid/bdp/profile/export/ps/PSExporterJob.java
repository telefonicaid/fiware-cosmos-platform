package es.tid.bdp.profile.export.ps;

import java.io.IOException;
import java.util.Date;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.log4j.Logger;

import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;

/**
 * Export user profiles to mongodb
 *
 * @author sortega
 */
public class PSExporterJob extends Job {
    private static final Logger LOG = Logger.getLogger(PSExporterJob.class);
    private static final String JOB_NAME = "PSExporterJob";
    private static final String OUTPUT_PREFIX = "part_";

    public PSExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(PSExporterJob.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setReducerClass(PSExporterReducer.class);
        PSExporterReducer.setTimestamp(this, new Date());
        this.setNumReduceTasks(1);
        this.setOutputKeyClass(NullWritable.class);
        this.setOutputValueClass(Text.class);
    }

    /**
     * Configure inputs and outputs.
     *
     * @param inputPath HDFS input path
     * @param outputPath HDFS output path
     *
     * @throws IOException
     */
    public void configure(Path inputPath, Path outputPath) throws IOException {
        this.setInputFormatClass(SequenceFileInputFormat.class);
        TextInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(TextOutputFormat.class);
        TextOutputFormat.setOutputPath(this, outputPath);
        DistributedCacheDictionary.cacheDictionary(this,
                DistributedCacheDictionary.LATEST_DICTIONARY);
    }

    @Override
    public void submit() throws IOException, InterruptedException,
                                ClassNotFoundException {
        super.submit();
        if (this.isSuccessful()) {
            this.renameOutput();
        }
    }

    protected void renameOutput() throws IOException {
        LOG.info("Renaming output file");
        Path outputPath = TextOutputFormat.getOutputPath(this);
        FileSystem fs = outputPath.getFileSystem(this.getConfiguration());
        Path srcPath = getOutputFile(fs, outputPath);
        Path destPath = PSExporterReducer.getOutputFileName(this);
        fs.rename(srcPath, destPath);
    }

    private static Path getOutputFile(FileSystem fs, Path outputPath)
            throws IOException {
        for (FileStatus status : fs.listStatus(outputPath)) {
            Path path = status.getPath();
            if (!status.isDir() && path.getName().startsWith(OUTPUT_PREFIX)) {
                return path;
            }
        }
        throw new IllegalArgumentException("Output file was not found");
    }

}
