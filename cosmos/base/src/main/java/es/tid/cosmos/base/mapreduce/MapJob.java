package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.InputFormat;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.OutputFormat;

/**
 *
 * @author ximo
 */
public final class MapJob extends CosmosJob {
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
                                       OutputKeyClass, OutputValueClass>,
            OutputFormatClass extends OutputFormat<OutputKeyClass,
                                                   OutputValueClass>,
            InputKeyClass, InputValueClass, OutputKeyClass, OutputValueClass>
        MapJob create(Configuration conf, String jobName,
            Class<InputFormatClass> inputFormat, Class<MapperClass> mapper,
            Class<OutputFormatClass> outputFormat)
            throws Exception {
        Class[] mapperClasses = getGenericParameters(mapper);
        return new MapJob(conf, jobName, inputFormat, mapper, mapperClasses[2],
                          mapperClasses[3], outputFormat);
    }

    private MapJob(Configuration conf, String jobName,
                   Class<? extends InputFormat> inputFormat,
                   Class<? extends Mapper> mapper,
                   Class<?> outputKey,
                   Class<?> outputValue,
                   Class<? extends OutputFormat> outputFormat)
            throws Exception {
        super(conf, jobName);

        this.setJarByClass(mapper);
        this.setInputFormatClass(inputFormat);
        this.setMapperClass(mapper);
        this.setMapOutputKeyClass(outputKey);
        this.setMapOutputValueClass(outputValue);
        this.setOutputKeyClass(outputKey);
        this.setOutputValueClass(outputValue);
        this.setOutputFormatClass(outputFormat);
    }
}
