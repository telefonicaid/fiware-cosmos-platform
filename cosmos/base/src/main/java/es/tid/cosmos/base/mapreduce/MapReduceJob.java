package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.InputFormat;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.OutputFormat;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author ximo
 */
public class MapReduceJob extends CosmosJob {
    // This class is created through a static method to enable type inference in
    // generic parameters by the Java compiler. Directs calls to constructors do
    // not activate the type inference mechanism in the compiler, which causes
    // the compiler to be called with Object as all its type parameters.
    //
    // This way, if you incorrectly call the creation function, you get a
    // compile-time error instead of a runtime error.
    public static <
            InputFormatClass extends InputFormat<InputKeyClass, InputValueClass>,
            MapperClass extends Mapper<InputKeyClass, InputValueClass,
                                       MapOutputKeyClass, MapOutputValueClass>,
            ReducerClass extends Reducer<MapOutputKeyClass, MapOutputValueClass,
                                         OutputKeyClass, OutputValueClass>,
            OutputFormatClass extends OutputFormat<OutputKeyClass,
                                                   OutputValueClass>,
            InputKeyClass, InputValueClass, MapOutputKeyClass,
            MapOutputValueClass, OutputKeyClass, OutputValueClass>
        MapReduceJob create(Configuration conf, String jobName,
            Class<InputFormatClass> inputFormat, Class<MapperClass> mapper,
            Class<ReducerClass> reducer, Class<OutputFormatClass> outputFormat)
            throws Exception {
        return create(conf, jobName, inputFormat, mapper, reducer, null,
                      outputFormat);
    }

    public static <
            InputFormatClass extends InputFormat<InputKeyClass, InputValueClass>,
            MapperClass extends Mapper<InputKeyClass, InputValueClass,
                                       MapOutputKeyClass, MapOutputValueClass>,
            ReducerClass extends Reducer<MapOutputKeyClass, MapOutputValueClass,
                                         OutputKeyClass, OutputValueClass>,
            OutputFormatClass extends OutputFormat<OutputKeyClass,
                                                   OutputValueClass>,
            InputKeyClass, InputValueClass, MapOutputKeyClass,
            MapOutputValueClass, OutputKeyClass, OutputValueClass>
        MapReduceJob create(Configuration conf, String jobName,
            Class<InputFormatClass> inputFormat, Class<MapperClass> mapper,
            Class<ReducerClass> reducer, Integer numReduceTasks,
            Class<OutputFormatClass> outputFormat)
            throws Exception {
        Class[] reducerClasses = getGenericParameters(reducer);
        return new MapReduceJob(conf, jobName, inputFormat, mapper,
                                reducerClasses[0], reducerClasses[1], reducer,
                                numReduceTasks, reducerClasses[2],
                                reducerClasses[3], outputFormat);
    }

    private MapReduceJob(Configuration conf, String jobName,
                         Class<? extends InputFormat> inputFormat,
                         Class<? extends Mapper> mapper,
                         Class<?> mapOutputKey,
                         Class<?> mapOutputValue,
                         Class<? extends Reducer> reducer,
                         Integer numReduceTasks,
                         Class<?> outputKey,
                         Class<?> outputValue,
                         Class<? extends OutputFormat> outputFormat)
            throws Exception {
        super(conf, jobName);

        this.setJarByClass(mapper);
        this.setInputFormatClass(inputFormat);
        this.setMapperClass(mapper);
        this.setMapOutputKeyClass(mapOutputKey);
        this.setMapOutputValueClass(mapOutputValue);
        this.setReducerClass(reducer);
        this.setOutputKeyClass(outputKey);
        this.setOutputValueClass(outputValue);
        this.setOutputFormatClass(outputFormat);

        if (numReduceTasks != null) {
            this.setNumReduceTasks(numReduceTasks);
        }
    }
}
