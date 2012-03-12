package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;

import com.hadoop.mapreduce.LzoTextInputFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class CategoryExtractionJobTest {
    private CategoryExtractionJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new CategoryExtractionJob(new Configuration());
    }
    
    @Test
    public void testConfigureTextInput() throws ClassNotFoundException {
        this.instance.configureTextInput();
        assertEquals(LzoTextInputFormat.class,
                     this.instance.getInputFormatClass());
        assertEquals(TextCategoryExtractionMapper.class,
                     this.instance.getMapperClass());
    }

    @Test
    public void testConfigureProtobufInput() throws ClassNotFoundException {
        this.instance.configureProtobufInput();
        assertEquals(ProtobufCategoryExtractionMapper.class,
                     this.instance.getMapperClass());
    }

    @Test
    public void testConfigurePaths() throws IOException {
        Path path1 = new Path("file:/opt/test1");
        Path path2 = new Path("file:/opt/test2");
        this.instance.configurePaths(path1, path2);
        Path[] inputPaths = FileInputFormat.getInputPaths(this.instance);
        assertEquals(1, inputPaths.length);
        assertEquals(path1, inputPaths[0]);
        assertEquals(path2, FileOutputFormat.getOutputPath(this.instance));
    }
}
