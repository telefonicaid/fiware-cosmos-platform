package es.tid.cosmos.mobility;

import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class MobilityConfigurationTest {
    private MobilityConfiguration instance;
    
    @Before
    public void setUp() throws IOException {
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        this.instance = new MobilityConfiguration();
        this.instance.load(configInput);
    }

    @Test
    public void testGetSysInputFolder() {
        assertEquals("/use/hdfs/mobility/input",
                     this.instance.getSysInputFolder());
    }

    @Test
    public void testGetSysOutputTrainingFolder() {
        assertEquals("/use/hdfs/mobility/output/training",
                     this.instance.getSysOutputTrainingFolder());
    }

    @Test
    public void testGetSysOutputCompleteFolder() {
        assertEquals("/use/hdfs/mobility/output/complete",
                     this.instance.getSysOutputCompleteFolder());
    }

    @Test
    public void testGetSysExecMode() {
        assertEquals("complete",
                     this.instance.getSysExecMode());
    }

    @Test
    public void testGetSysExecIncremental() {
        assertEquals(false,
                     this.instance.getSysExecIncremental());
    }

    @Test
    public void testGetDataSeparator() {
        assertEquals("|",
                     this.instance.getDataSeparator());
    }

    @Test
    public void testGetDataDateFormat() {
        assertEquals("YYYYMMDD",
                     this.instance.getDataDateFormat());
    }

    @Test
    public void testGetDataStartDate() {
        assertEquals("20120527",
                     this.instance.getDataStartDate());
    }

    @Test
    public void testGetDataEndDate() {
        assertEquals("20120528",
                     this.instance.getDataEndDate());
    }

    @Test
    public void testGetClientMinTotalCalls() {
        assertEquals(200,
                     this.instance.getClientMinTotalCalls());
    }

    @Test
    public void testGetClientMaxTotalCalls() {
        assertEquals(5000,
                     this.instance.getClientMaxTotalCalls());
    }

    @Test
    public void testGetBtsMaxBtsArea() {
        assertEquals(4.2D,
                     this.instance.getBtsMaxBtsArea(), 0.001D);
    }

    @Test
    public void testGetBtsMinCommsBts() {
        assertEquals(70000,
                     this.instance.getBtsMinCommsBts());
    }

    @Test
    public void testGetPoiHomeLabelgroupId() {
        assertEquals(3,
                     this.instance.getPoiHomeLabelgroupId());
    }

    @Test
    public void testGetPoiMinDistSecondHome() {
        assertEquals(50165.21D,
                     this.instance.getPoiMinDistSecondHome(), 0.001D);
    }

    @Test
    public void testGetPoiMinPercRepBts() {
        assertEquals(5,
                     this.instance.getPoiMinPercRepBts());
    }

    @Test
    public void testGetPoiMinNumberCallsBts() {
        assertEquals(12,
                     this.instance.getPoiMinNumberCallsBts());
    }

    @Test
    public void testGetItinPercAbsoluteMax() {
        assertEquals(20.0D,
                     this.instance.getItinPercAbsoluteMax(), 0.001D);
    }

    @Test
    public void testGetItinMaxMinutesInMoves() {
        assertEquals(360,
                     this.instance.getItinMaxMinutesInMoves());
    }

    @Test
    public void testGetItinMinMinutesInMoves() {
        assertEquals(0,
                     this.instance.getItinMinMinutesInMoves());
    }

    @Test
    public void testGetItinMinMoves() {
        assertEquals(5.9D,
                     this.instance.getItinMinMoves(), 0.001D);
    }

    @Test
    public void testGetMtxMaxMinutesInMoves() {
        assertEquals(360,
                     this.instance.getMtxMaxMinutesInMoves());
    }

    @Test
    public void testGetMtxMinMinutesInMoves() {
        assertEquals(0,
                     this.instance.getMtxMinMinutesInMoves());
    }

    @Test
    public void testGetMtxIncludeIntraMoves() {
        assertEquals(false,
                     this.instance.getMtxIncludeIntraMoves());
    }
}
