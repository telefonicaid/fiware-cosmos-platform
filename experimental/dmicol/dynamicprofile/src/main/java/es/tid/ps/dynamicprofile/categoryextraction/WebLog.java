// <editor-fold defaultstate="collapsed" desc="Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.">
//
//   File        : WebLog.java
//
//   Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.
//
//   The copyright to the file(s) is property of Telefonica I+D.
//   The file(s) may be used and or copied only with the express written
//   consent of Telefonica I+D or in accordance with the terms and conditions
//   stipulated in the agreement/contract under which the files(s) have
//   been supplied.
//
// </editor-fold>
package es.tid.ps.dynamicprofile.categoryextraction;

public class WebLog {
    private static final String DELIMITER ="\t";
    public String visitorId;
    public String fullUrl;

    public void set(String line) throws IllegalArgumentException, SecurityException, IllegalAccessException, NoSuchFieldException {
        String[] fields = line.split(DELIMITER);

        this.visitorId = fields[0];
        this.fullUrl = fields[2];
    }
}
