package es.tid.mapreduce.mobility2;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.mapreduce.mobility2.data.AreaOfInfluence;
import es.tid.mapreduce.mobility2.data.CellData;

@RunWith(BlockJUnit4ClassRunner.class)
public class AreaOfInfluenceTest extends TestCase {

    @Test
    public void generateAreaOfInfluenceTest() {
        String indentifer = "identifer";
        List<CellData> elements = new ArrayList<CellData>();

        elements.add(new CellData("cell1", "bts1", "state", "country",
                new Double(-102.296619), new Double(21.8809889)));
        elements.add(new CellData("cell2", "bts2", "state", "country",
                new Double(-102.304581), new Double(21.8771694)));
        elements.add(new CellData("cell3", "bts3", "state", "country",
                new Double(-102.295608), new Double(21.8744667)));
        elements.add(new CellData("cell4", "bts4", "state", "country",
                new Double(-102.280553), new Double(21.8788861)));
        elements.add(new CellData("cell5", "bts5", "state", "country",
                new Double(-102.297358), new Double(21.8892722)));

        AreaOfInfluence actual = AreaOfInfluence.calculateCenterOfMass(
                indentifer, elements);

        AreaOfInfluence expected = new AreaOfInfluence(indentifer,
                new Double(21.88015666), new Double(-102.29494380000001), new Double(1.414215715633228));

        assertEquals(expected, actual);

    }

}
