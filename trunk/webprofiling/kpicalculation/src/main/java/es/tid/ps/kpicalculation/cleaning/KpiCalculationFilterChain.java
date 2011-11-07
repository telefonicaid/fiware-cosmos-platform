package es.tid.ps.kpicalculation.cleaning;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.InvalidPropertiesFormatException;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Properties;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 * Abstract class to implement the "chain of responsibility" pattern. This
 * pattern is used apply different filters to the input data, to asses if it
 * will be useful for calculating kpi's for the personalisation process.
 * 
 * @author javierb
 * 
 */
public class KpiCalculationFilterChain {

    private List<IKpiCalculationFilter> handlers;
    private static String COLLECTION_ID = "kpifilters";
    

    public KpiCalculationFilterChain() {
        Configuration conf = new Configuration();
        conf.addResource("kpi-filtering.xml");
        
        handlers = new ArrayList<IKpiCalculationFilter>();
        Collection<String> classes = conf.getStringCollection(COLLECTION_ID);
        Iterator<String> it = classes.iterator();
        while (it.hasNext())
        {
            IKpiCalculationFilter filter;
            try {
                filter = (IKpiCalculationFilter) Class.forName(it.next().trim()).newInstance();
                handlers.add(filter);
            } catch (InstantiationException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (ClassNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    public void filter(String url) throws Exception {
        IKpiCalculationFilter currentFilter;
        ListIterator<IKpiCalculationFilter> it = handlers.listIterator();

        while (it.hasNext()) {
            currentFilter = it.next();
            currentFilter.filter(url);
        }
    }
}
