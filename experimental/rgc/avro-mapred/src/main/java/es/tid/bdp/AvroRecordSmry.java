package es.tid.bdp;

import java.io.IOException;

import org.apache.avro.Schema;
import org.apache.avro.Schema.Type;
import org.apache.avro.generic.GenericData.Record;
import org.apache.avro.mapred.*;
import org.apache.avro.reflect.ReflectData;
import org.apache.avro.util.Utf8;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.*;

public class AvroRecordSmry extends Configured implements Tool {
    private static final Schema LONG_SCHEMA = Schema.create(Type.LONG);
    private static final Schema TEXT_SCHEMA = Schema.create(Type.STRING);
    //public static final Schema IN_SCHEMA = ReflectData.get().getSchema(Record.class);
    public static final Schema OUT_SCHEMA = new Pair<Utf8,Long>(new Utf8(), TEXT_SCHEMA, 0L, LONG_SCHEMA).getSchema();
        
    public int run(String[] args) throws Exception {

        JobConf conf = new JobConf();
        String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
        if (otherArgs.length != 2) {
            System.err.println("Usage: avro record smry <in> <out>");
            System.exit(2);
        }
        Job job = new Job(conf, "avro record count");
        job.setJarByClass(AvroRecordSmry.class);
        FileInputFormat.setInputPaths(job, new Path(otherArgs[0]));
        Path outputPath = new Path(otherArgs[1]);
        FileOutputFormat.setOutputPath(job, outputPath);
        
        outputPath.getFileSystem(conf).delete(outputPath);
        
        AvroJob.setInputReflect(conf) ;
        AvroJob.setOutputSchema(conf, OUT_SCHEMA);
                
        AvroJob.setMapperClass(conf, AvroRecordMapper.class);        
        AvroJob.setCombinerClass(conf, AvroRecordReducer.class);
        AvroJob.setReducerClass(conf, AvroRecordReducer.class);
        
        FileOutputFormat.setCompressOutput(job, true); //?
        
        // setMeta(job);

        return job.waitForCompletion(true) ? 0 : 1;
    }

    public static class AvroRecordMapper extends AvroMapper<Record,Pair<Record,Long>> {

        // requires Avro trunk - Avro mapreduce support isn't yet released
        public void map(Record in, AvroCollector<Pair<Utf8,Long>> collector, Reporter reporter) throws IOException {
            Pair<Utf8,Long> p = new Pair<Utf8,Long>(OUT_SCHEMA);
            p.set((Utf8) in.get("tfnoTemm"), 1L);
            collector.collect(p);
        }      
    }

    public static class AvroRecordReducer extends AvroReducer<Utf8, Long, Pair<Utf8,Long>> {

        public void reduce(Utf8 key, Iterable<Long> values,
                AvroCollector<Pair<Utf8,Long>> collector,
                Reporter reporter) throws IOException {
            Pair<Utf8,Long> p = new Pair<Utf8,Long>(OUT_SCHEMA);
            long sum = 0;
            for (Long val : values) {
                sum += val;
            }
            System.out.println(key);
            p.set( key, sum);
            collector.collect(p);
        }
    }
    
    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new AvroRecordSmry(), args);
        System.exit(res);
    }

}