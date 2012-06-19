package es.tid.cosmos.mobility.util;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 *
 * @author dmicol
 */
public class CentroidsCatalogueTest {
    @Test
    public void testLoad() throws IOException {
        String text = "1|2|0.011890149404647|0.016823651640848|" 
                + "0.00324639244853738|0.00134727223726977|0.000651018555796316|"
                + "0.000412335701300108|0.000434298491874323|"
                + "0.000889176445287108|0.0023228228580715|0.00531692656554713|"
                + "0.00918212723726978|0.0127080478656555|0.0147025893824485|"
                + "0.0155759929144095|0.0163328056338028|0.0169369702058505|"
                + "0.016595961971831|0.0158949014084507|0.0158148747562297|"
                + "0.0162419535211268|0.0173135919826652|0.0176461652221019|"
                + "0.0174248722643554|0.0160859156338028|0.0124278291332611|"
                + "0.00709241452871073|0.00405373759479957|0.00194125086890574|"
                + "0.00106243297399783|0.000698406189599135|"
                + "0.000615116950162514|0.00100763816359697|0.00239823040411701|"
                + "0.00539197217768147|0.0091889500866739|0.0125713424160347|"
                + "0.0146508700108342|0.015776053304442|0.0167224306608884|"
                + "0.0174293322860238|0.0172508241603467|0.0167590659804984|"
                + "0.0166891687973998|0.0172511444203684|0.0185722729144095|"
                + "0.0193002410617551|0.0192987139761647|0.0178610851570964|"
                + "0.0143093342470206|0.00887617881906825|0.00461372702058505|"
                + "0.00230873312567714|0.00130733075297942|0.000891573218851571|"
                + "0.000739245428602383|0.000968515742145178|"
                + "0.00191881722318527|0.00438053856988083|0.00830116449620802|"
                + "0.0124980742145179|0.0152194014192849|0.0165933386998917|"
                + "0.0174439769230769|0.0179879981581798|0.0181778489707476|"
                + "0.0175604044420368|0.0166370940411701|0.0161083156013001|"
                + "0.0164127754062839|0.0171889450704225|0.0175174820151679|"
                + "0.016672516468039|0.0137708566847237|0.00931736304442037|"
                + "0.00550527983748646|0.00313510453196099|0.00195803504008667|"
                + "0.00135276371180932|0.000973038364030334|"
                + "0.000932159587215601|0.00145479999024919|0.00317872895124594|"
                + "0.00616553541711809|0.00966168738894908|0.0121309944420368|"
                + "0.0132970050379198|0.0136773684290358|0.0137427796966414|"
                + "0.013686780780065|0.0132386190790899|0.012546750595883|"
                + "0.0122207242145179|0.0127380859263272|0.0137016352221018|"
                + "0.0145254067172264|0.0143057717118093|0.0118448756663055|"
                + "0.00721698692307692\n"
                + "3|4|0.011890149404647|0.016823651640848|" 
                + "0.00324639244853738|0.00134727223726977|0.000651018555796316|"
                + "0.000412335701300108|0.000434298491874323|"
                + "0.000889176445287108|0.0023228228580715|0.00531692656554713|"
                + "0.00918212723726978|0.0127080478656555|0.0147025893824485|"
                + "0.0155759929144095|0.0163328056338028|0.0169369702058505|"
                + "0.016595961971831|0.0158949014084507|0.0158148747562297|"
                + "0.0162419535211268|0.0173135919826652|0.0176461652221019|"
                + "0.0174248722643554|0.0160859156338028|0.0124278291332611|"
                + "0.00709241452871073|0.00405373759479957|0.00194125086890574|"
                + "0.00106243297399783|0.000698406189599135|"
                + "0.000615116950162514|0.00100763816359697|0.00239823040411701|"
                + "0.00539197217768147|0.0091889500866739|0.0125713424160347|"
                + "0.0146508700108342|0.015776053304442|0.0167224306608884|"
                + "0.0174293322860238|0.0172508241603467|0.0167590659804984|"
                + "0.0166891687973998|0.0172511444203684|0.0185722729144095|"
                + "0.0193002410617551|0.0192987139761647|0.0178610851570964|"
                + "0.0143093342470206|0.00887617881906825|0.00461372702058505|"
                + "0.00230873312567714|0.00130733075297942|0.000891573218851571|"
                + "0.000739245428602383|0.000968515742145178|"
                + "0.00191881722318527|0.00438053856988083|0.00830116449620802|"
                + "0.0124980742145179|0.0152194014192849|0.0165933386998917|"
                + "0.0174439769230769|0.0179879981581798|0.0181778489707476|"
                + "0.0175604044420368|0.0166370940411701|0.0161083156013001|"
                + "0.0164127754062839|0.0171889450704225|0.0175174820151679|"
                + "0.016672516468039|0.0137708566847237|0.00931736304442037|"
                + "0.00550527983748646|0.00313510453196099|0.00195803504008667|"
                + "0.00135276371180932|0.000973038364030334|"
                + "0.000932159587215601|0.00145479999024919|0.00317872895124594|"
                + "0.00616553541711809|0.00966168738894908|0.0121309944420368|"
                + "0.0132970050379198|0.0136773684290358|0.0137427796966414|"
                + "0.013686780780065|0.0132386190790899|0.012546750595883|"
                + "0.0122207242145179|0.0127380859263272|0.0137016352221018|"
                + "0.0145254067172264|0.0143057717118093|0.0118448756663055|"
                + "0.00721698692307692\n";
        List<Cluster> centroids = CentroidsCatalogue.load(
                new StringReader(text), "\\|");
        assertNotNull(centroids);
        assertEquals(2, centroids.size());
        assertEquals(1, centroids.get(0).getLabel());
        assertEquals(4, centroids.get(1).getLabelgroup());
    }
}
