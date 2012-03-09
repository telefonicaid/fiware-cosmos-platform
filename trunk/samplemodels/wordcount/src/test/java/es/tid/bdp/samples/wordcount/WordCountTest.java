//   Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.
//
//   The copyright to the file(s) is property of Telefonica I+D.
//   The file(s) may be used and or copied only with the express written
//   consent of Telefonica I+D or in accordance with the terms and conditions
//   stipulated in the agreement/contract under which the files(s) have
//   been supplied.
package es.tid.bdp.samples.wordcount;

import static org.hamcrest.Matchers.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;
import org.junit.Before;
import org.junit.Test;

import org.apache.hadoop.io.*;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;

import es.tid.bdp.samples.wordcount.WordCountMapper;
import es.tid.bdp.samples.wordcount.WordCountReducer;

/**
 * Test case for WordCount sample model
 *
 * @author logc
 */
public class WordCountTest {

    private WordCountMapper mapper;
    private WordCountReducer reducer;
    private MapReduceDriver driver;

    @Before
    public void setUp() {
        this.mapper = new WordCountMapper();
        this.reducer = new WordCountReducer();
        this.driver = new MapReduceDriver<LongWritable, Text, Text,
            IntWritable, Text, IntWritable>();
    }

    @Test
    public void shouldCountWords() {
        Text a_short_text = new Text("una cadena de texto donde se repite una palabra");

        Text una = new Text("una");
        Text cadena = new Text("cadena");
        Text de = new Text("de");
        Text texto = new Text("texto");
        Text donde = new Text("donde");
        Text se = new Text("se");
        Text repite = new Text("repite");
        Text palabra = new Text("palabra");

        this.driver
            .withInput(new LongWritable(1), a_short_text)
            .withMapper(this.mapper)
            .withReducer(this.reducer)
            .withOutput(cadena, new IntWritable(1))
            .withOutput(de, new IntWritable(1))
            .withOutput(donde, new IntWritable(1))
            .withOutput(palabra, new IntWritable(1))
            .withOutput(repite, new IntWritable(1))
            .withOutput(se, new IntWritable(1))
            .withOutput(texto, new IntWritable(1))
            .withOutput(una, new IntWritable(2)) // this is the repeated word
            .runTest();

    }
}
