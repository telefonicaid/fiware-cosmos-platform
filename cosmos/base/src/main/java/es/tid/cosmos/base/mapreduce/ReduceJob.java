package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.InputFormat;
import org.apache.hadoop.mapreduce.OutputFormat;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author ximo
 */
public class ReduceJob extends CosmosJob {
    // This class is created through a static method to enable type inference in
    // generic parameters by the Java compiler. Directs calls to constructors do
    // not activate the type inference mechanism in the compiler, which causes
    // the compiler to be called with Object as all its type parameters.
    //
    // This way, if you incorrectly call the creation function, you get a
    // compile-time error instead of a runtime error.
    public static <
            InputFormatClass extends InputFormat<InputKeyClass, InputValueClass>,
            ReducerClass extends Reducer<InputKeyClass, InputValueClass,
                                         OutputKeyClass, OutputValueClass>,
            OutputFormatClass extends OutputFormat<OutputKeyClass,
                                                   OutputValueClass>,
            InputKeyClass, InputValueClass, OutputKeyClass, OutputValueClass>
        ReduceJob create(Configuration conf, String jobName,
            Class<InputFormatClass> inputFormat, Class<ReducerClass> reducer,
            Class<OutputFormatClass> outputFormat)
            throws Exception {
        return create(conf, jobName, inputFormat, reducer, null, outputFormat);
    }

    public static <
            InputFormatClass extends InputFormat<InputKeyClass, InputValueClass>,
            ReducerClass extends Reducer<InputKeyClass, InputValueClass,
                                         OutputKeyClass, OutputValueClass>,
            OutputFormatClass extends OutputFormat<OutputKeyClass,
                                                   OutputValueClass>,
            InputKeyClass, InputValueClass, OutputKeyClass, OutputValueClass>
        ReduceJob create(Configuration conf, String jobName,
            Class<InputFormatClass> inputFormat, Class<ReducerClass> reducer,
            Integer numReduceTasks, Class<OutputFormatClass> outputFormat)
            throws Exception {
        Class[] reducerClasses = getGenericParameters(reducer);
        return new ReduceJob(conf, jobName, inputFormat, reducerClasses[0],
                             reducerClasses[1], reducer, numReduceTasks,
                             reducerClasses[2], reducerClasses[3], outputFormat);
    }

    private ReduceJob(Configuration conf, String jobName,
                      Class<? extends InputFormat> inputFormat,
                      Class<?> inputKey,
                      Class<?> inputValue,
                      Class<? extends Reducer> reducer,
                      Integer numReduceTasks,
                      Class<?> outputKey,
                      Class<?> outputValue,
                      Class<? extends OutputFormat> outputFormat)
            throws Exception {
        super(conf, jobName);

        this.setJarByClass(reducer);
        this.setInputFormatClass(inputFormat);
        this.setMapOutputKeyClass(inputKey);
        this.setMapOutputValueClass(inputValue);
        this.setReducerClass(reducer);
        this.setOutputKeyClass(outputKey);
        this.setOutputValueClass(outputValue);
        this.setOutputFormatClass(outputFormat);

        if(numReduceTasks != null) {
            this.setNumReduceTasks(numReduceTasks);
        }
    }
}
