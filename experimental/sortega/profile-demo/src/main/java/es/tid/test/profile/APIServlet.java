package es.tid.test.profile;

import com.mongodb.DB;
import com.mongodb.Mongo;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author sortega
 */
public class APIServlet extends HttpServlet {

    private static final String PROFILE_COLLECTION = "profile";
    private Mongo mongo;
    private ProfileDAO profile;

    @Override
    public void init(ServletConfig config) throws ServletException {
        try {
            this.mongo = new Mongo(config.getInitParameter("db.hostname"));
        } catch (Exception ex) {
            throw new RuntimeException("Cannot conntect to mongodb", ex);
        }
        DB db = this.mongo.getDB(config.getInitParameter("db.name"));
        this.profile = new ProfileDAO(db.getCollection(PROFILE_COLLECTION));
    }

    @Override
    public void destroy() {
        this.mongo.close();
    }

    /**
     * Processes requests for both HTTP
     * <code>GET</code> and
     * <code>POST</code> methods.
     *
     * @param request  servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException      if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request,
                                  HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("application/json;charset=UTF-8");
        PrintWriter out = response.getWriter();
        try {
            String userId = request.getParameter("user");
            int n = this.getIntParameter(request, "top", 3);

            CategoryMap categoryMap = profile.getLastCategoryMap(userId);
            final List<String> topN = categoryMap.getTop(n);
            if (topN.isEmpty()) {
                out.print("[]");
            } else {
                out.format("[\"%s\"]", StringUtil.join("\", \"", topN));
            }
        } finally {
            out.close();
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP
     * <code>GET</code> method.
     *
     * @param request  servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException      if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request,
                         HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Handles the HTTP
     * <code>POST</code> method.
     *
     * @param request  servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException      if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request,
                          HttpServletResponse response)
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

    private int getIntParameter(HttpServletRequest request, String name, int fallback) {
        String value = request.getParameter(name);
        try {
            return Integer.parseInt(value, 10);
        } catch(NumberFormatException unused) {
            return fallback;
        }
    }
}
