package es.tid.analytics.mobility.core.test;

import es.tid.analytics.mobility.core.data.Cdr;
import es.tid.analytics.mobility.core.data.Cell;
import es.tid.analytics.mobility.core.parsers.ParserCdr;
import es.tid.analytics.mobility.core.parsers.ParserCell;
import es.tid.analytics.mobility.core.parsers.ParserFactory;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import java.util.Date;

/**
 * User: masp20
 * Date: 26-ene-2012
 * Time: 16:58:30
 */
public class ParserTest {

    @Test
    public void testDefaultParserCellCorrect() {
        final Cell testCell = new Cell();
        testCell.setIdCell(65570011L);
        testCell.setGeoLocationLevel1(65570011L);
        testCell.setGeoLocationLevel2(13401L);
        testCell.setGeoLocationLevel3(1000L);
        testCell.setGeoLocationLevel4(1L);
        testCell.setLatitude(-102.296619);
        testCell.setLongitude(21.8809889);


        final ParserCell cellParser = new ParserFactory().createNewDefaultCellParser();
        final Cell cell = cellParser.parseCellLine("65570011|13401|1000|1|-102.296619|21.8809889");

        assertEquals("Incorrect Cell obtained", testCell, cell);
    }

    @Test
    public void testParserCellCorrect() {
        final Cell testCell = new Cell();
        testCell.setIdCell(65570011L);
        testCell.setGeoLocationLevel1(65570011L);
        testCell.setGeoLocationLevel2(13401L);
        testCell.setGeoLocationLevel3(1000L);
        testCell.setGeoLocationLevel4(1L);
        testCell.setLatitude(-102.296619);
        testCell.setLongitude(21.8809889);


        final ParserCell cellParser = new ParserFactory().createNewCellParser(ParserFactory.PARSER_DEFAULT);
        final Cell cell = cellParser.parseCellLine("65570011|13401|1000|1|-102.296619|21.8809889");

        assertEquals("Incorrect Cell obtained", testCell, cell);
    }

    @Test
    public void testParserAlphaCellCorrect() {
        final Cell testCell = new Cell();
        testCell.setIdCell(29783884378L);
        testCell.setGeoLocationLevel1(29783884378L);
        testCell.setGeoLocationLevel2(1102003721956L);
        testCell.setGeoLocationLevel3(7L);
        testCell.setGeoLocationLevel4(21755941L);
        testCell.setLatitude(-102.296619);
        testCell.setLongitude(21.8809889);


        final ParserCell cellParser = new ParserFactory().createNewCellParser(ParserFactory.PARSER_ALPHA);
        final Cell cell = cellParser.parseCellLine("ALHUEU1|ALHUEU06|6|ALHUE|-102.296619|21.8809889");

        assertEquals("Incorrect Cell obtained", testCell, cell);
    }

    @Test
    public void testDefaultParserCDRCorrect() {
        final Cdr testCdr = new Cdr();
        final Date testDate = new Date(1262622067000L);

        testCdr.setNode(2221435146L);
        testCdr.setIdCell(1377205526L);
        testCdr.setDate(testDate);

        final ParserCdr parserCdr = new ParserFactory().createNewDefaultCdrParser();
        final Cdr cdr = parserCdr.parseCdrsLine("1377205526|2221435146|1377205526|0442221472843|2|LDN|20100104|17:21:07|22|118-TELEFONIA MOVIL|118-TELEFONIA MOVIL|??|??|11115006528440|NOROAMI");

        assertEquals("Incorrect Cdr obtained", testCdr, cdr);
    }

    @Test
    public void testParserCDRCorrect() {
        final Cdr testCdr = new Cdr();
        final Date testDate = new Date(1262622067000L);

        testCdr.setNode(2221435146L);
        testCdr.setIdCell(1377205526L);
        testCdr.setDate(testDate);

        final ParserCdr parserCdr = new ParserFactory().createNewCdrParser(ParserFactory.PARSER_DEFAULT);
        final Cdr cdr = parserCdr.parseCdrsLine("1377205526|2221435146|1377205526|0442221472843|2|LDN|20100104|17:21:07|22|118-TELEFONIA MOVIL|118-TELEFONIA MOVIL|??|??|11115006528440|NOROAMI");

        assertEquals("Incorrect Cdr obtained", testCdr, cdr);
    }

    @Test
    public void testParserAlphaCDRCorrect() {
        final Cdr testCdr = new Cdr();
        final Date testDate = new Date(1262622067000L);

        testCdr.setNode(2221435146L);
        testCdr.setIdCell(29783884378L);
        testCdr.setDate(testDate);

        final ParserCdr parserCdr = new ParserFactory().createNewCdrParser(ParserFactory.PARSER_ALPHA);
        final Cdr cdr = parserCdr.parseCdrsLine("ALHUEU1|2221435146|ALHUEU1|0442221472843|2|LDN|20100104|17:21:07|22|118-TELEFONIA MOVIL|118-TELEFONIA MOVIL|??|??|11115006528440|NOROAMI");

        assertEquals("Incorrect Cdr obtained", testCdr, cdr);
    }

    @Test(expected = NullPointerException.class)
    public void testNullPointerException() {
        new ParserFactory().createNewCdrParser("FAIL");
    }

}
