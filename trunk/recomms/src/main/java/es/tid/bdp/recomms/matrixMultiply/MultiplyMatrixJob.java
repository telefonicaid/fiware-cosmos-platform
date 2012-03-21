package es.tid.bdp.recomms.matrixMultiply;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.ToolRunner;
import org.apache.mahout.common.AbstractJob;
import es.tid.bdp.recomms.matrixMultiply.PartialMultiply.ComputePartialMultiplyJob;
import es.tid.bdp.recomms.matrixMultiply.endMultiplication.ComputeFinishMultiplyJob;
import es.tid.bdp.recomms.matrixMultiply.userVectorSplits.ComputeUserVectorSplitsJob;

import java.util.Map;

/*
 *      start the multiplication of the co-occurrence matrix by the user vectors
 *      @author Jaume
 */
public class MultiplyMatrixJob extends AbstractJob {

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new MultiplyMatrixJob(), args);
    }

    @Override
    public int run(String[] args) throws Exception {

        addInputOption();
        addOutputOption();
        addOption("booleanData", "b", "boolean or Not",
                Boolean.FALSE.toString());
        addOption("maxPrefsPerUser", "maxPrefs",
                "max number of preferences to consider per user");
        addOption("userVectors", "uv", "user vector dir",
                Boolean.TRUE.toString());

        Map<String, String> parsedArgs = parseArguments(args);
        if (parsedArgs == null) {
            return -1;
        }

        boolean booleanData = Boolean.parseBoolean(parsedArgs.get("--boolean"));
        int maxPrefsPerUser = Integer.parseInt(parsedArgs
                .get("--maxPrefsPerUser"));
        String userVectors = String.valueOf(parsedArgs.get("--userVectors"));

        Path prePartialMultiplyPath1 = getTempPath("prePartialMultiply1");
        Path prePartialMultiplyPath2 = getTempPath("prePartialMultiply2");

        ComputePartialMultiplyJob computePartialMult = new ComputePartialMultiplyJob(
                this.getConf());
        computePartialMult.configure(getInputPath(), prePartialMultiplyPath1,
                booleanData);
        if (!computePartialMult.waitForCompletion(true)) {
            return 1;
        }

        ComputeUserVectorSplitsJob computeUVSplits = new ComputeUserVectorSplitsJob(
                this.getConf());
        computeUVSplits.configure(getTempPath(userVectors),
                prePartialMultiplyPath2, maxPrefsPerUser);
        if (!computeUVSplits.waitForCompletion(true)) {
            return 1;
        }

        ComputeFinishMultiplyJob computFinalMultiply = new ComputeFinishMultiplyJob(
                this.getConf());
        computFinalMultiply.configure(getTempPath(), prePartialMultiplyPath1,
                prePartialMultiplyPath2, getOutputPath());
        if (!computFinalMultiply.waitForCompletion(true)) {
            return 1;
        }
        return 0;
    }
}
