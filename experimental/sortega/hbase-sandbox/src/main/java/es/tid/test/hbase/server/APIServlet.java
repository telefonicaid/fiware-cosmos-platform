package es.tid.test.hbase.server;

import es.tid.test.hbase.CategoryMap;
import es.tid.test.hbase.CategoryMapLookup;
import es.tid.test.hbase.Main;
import es.tid.test.hbase.StringUtil;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.HTable;

/**
 *
 * @author sebastian
 */
public class APIServlet extends HttpServlet {
    
    private static final int N = 3;
    private static CategoryMapLookup lookup;

    @Override
    public void init(ServletConfig cfg) throws ServletException {
        Configuration config = HBaseConfiguration.create();
        config.addResource(Main.class.getResource("config.xml"));
        try {
            HTable profileTable = new HTable(config, "individualprofile");
            lookup = new CategoryMapLookup(profileTable);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }
    
    /**
     * Processes requests for both HTTP
     * <code>GET</code> and
     * <code>POST</code> methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        
        response.setContentType("application/json;charset=UTF-8");
        PrintWriter out = response.getWriter();
        try {
            String userId = request.getParameter("user");
            
            CategoryMap categoryMap = lookup.getLastFor(userId);
            final List<String> topN = categoryMap.getTop(N);
            if (topN.isEmpty()) {
                out.print("[]");
            } else {
                out.format("[\"%s\"]", StringUtil.join("\", \"", topN));
            }
            
        }
        finally {            
            out.close();
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP
     * <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Handles the HTTP
     * <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Returns a short description of the servlet.
     *
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>
}
