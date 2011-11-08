package es.tid.ps.kpicalculation.cleaning;

import java.text.MessageFormat;
import java.util.StringTokenizer;

public abstract class AbstractKpiCalculationFilter implements
        IKpiCalculationFilter {

    @Override
    public abstract void filter(String url);
    
    protected String setPattern(String regExp, String values) {
        StringTokenizer stt = new StringTokenizer(values.trim(), "\n");
        StringBuilder filters = new StringBuilder("(");
        
        while (stt.hasMoreTokens())
            filters.append(stt.nextToken().trim().replace(".", "\\.")).append(
                    "|");
        
        filters.setCharAt(filters.length() -1, ')');
        return MessageFormat.format(regExp, filters.toString());
    }

}
