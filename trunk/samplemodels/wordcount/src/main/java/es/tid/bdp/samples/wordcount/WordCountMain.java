package es.tid.bdp.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configuration;
import org.apache.log4j.Logger;

/**
 * Application entry point. Runs a job that counts each different word
 * in a given text.
 *
 * @author logc
 */
public class WordCountMain extends Configured implements Tool {

    private static final Logger logger = Logger.getLogger(
            WordCountMain.class);

    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length != 2) {
            throw new IllegalArgumentException("Usage: text_path output_path\n");
        }
        Path textPath = new Path(args[0]);
        Path outPath = new Path(args[1]);

        WordCountJob wcJob = new WordCountJob(this.getConf());
        wcJob.configure(textPath, outPath);

        logger.info("wordcount started");
        if (!wcJob.waitForCompletion(true)) {
            logger.error("done with errors");
            return 1;
        }
        logger.info("done");
        return 0;
    }

    public static void main(String[] args) {
        try {
            int res = ToolRunner.run(new Configuration(),
                    new WordCountMain(), args);
            System.exit(res);
        } catch (Exception ex) {
            ex.printStackTrace(System.err);
            System.exit(1);
        }
    }
}
