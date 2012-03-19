package es.tid.bdp.recomms.extractRecomms;

import org.apache.hadoop.util.ToolRunner;
import org.apache.mahout.common.AbstractJob;

import java.util.Map;

/*
 *      Compute final predictions and sort recommendations
 *      @author Jaume
 */
public class GetRecommsJob extends AbstractJob {

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new GetRecommsJob(), args);
    }

    @Override
    public int run(String[] args) throws Exception {

        addInputOption();
        addOutputOption();
        addOption("booleanData", "b", "boolean or Not",
                Boolean.FALSE.toString());
        addOption("mongoOutput", "mongoOutput", "mongoOutput", Boolean.FALSE.toString());
        addOption("numRecos", "numRecoms",
                "Number of recommendations per users");

        Map<String, String> parsedArgs = parseArguments(args);
        if (parsedArgs == null) {
            return -1;
        }

        boolean booleanData = Boolean.parseBoolean(parsedArgs.get("--boolean"));
        boolean mongoOutput = Boolean.parseBoolean(parsedArgs
                .get("--mongoOutput"));
        int numRecoms = Integer.parseInt(parsedArgs.get("--numRecos"));

        ComputePredictionsJob computePreds = new ComputePredictionsJob(
                this.getConf());
        computePreds.configure(getInputPath(), getOutputPath(), numRecoms,
                booleanData, mongoOutput);
        if (!computePreds.waitForCompletion(true)) {
            return 1;
        }
        return 0;
    }
}
