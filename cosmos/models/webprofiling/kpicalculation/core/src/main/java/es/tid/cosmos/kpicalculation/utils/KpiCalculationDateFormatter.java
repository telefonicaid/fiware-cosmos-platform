package es.tid.cosmos.kpicalculation.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

/**
 * Class used to format the date string received in log data allowing to get the
 * date and time in the wanted format
 *
 * @author javierb
 */
public class KpiCalculationDateFormatter {
    private static final String DELIMITER = "/";
    
    private SimpleDateFormat inputFormat;
    private SimpleDateFormat dateFormat;
    private Calendar calendar;

    public KpiCalculationDateFormatter() {
        this.inputFormat = new SimpleDateFormat("ddMMMyyyyhhmmss",
                                                Locale.ENGLISH);
        this.dateFormat = new SimpleDateFormat("dd" + DELIMITER + "MM"
                                               + DELIMITER + "yyyy");
        this.calendar = Calendar.getInstance();
    }

    /**
     * Method that provides the formatted date string corresponding to the input
     *
     * @param inputDate
     *            String date to format
     * @return the formatted date
     */
    public String getValue(String inputDate) throws ParseException {
        Date date = this.inputFormat.parse(inputDate);
        this.calendar.setTime(date);
        return this.dateFormat.format(this.calendar.getTime());
    }
}
