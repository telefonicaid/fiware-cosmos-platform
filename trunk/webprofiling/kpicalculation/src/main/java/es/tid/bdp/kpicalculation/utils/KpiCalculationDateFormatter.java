package es.tid.bdp.kpicalculation.utils;

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
public final class KpiCalculationDateFormatter {
    private static boolean isInitialized = false;
    private static String currentDelimiter = null;
    private static SimpleDateFormat inputFormat = null;
    private static SimpleDateFormat dateFormat = null;
    private static Calendar calendar = null;

    private KpiCalculationDateFormatter() {
    }

    /**
     * Method that initializes the formatters
     */
    public static void init(String delimiter) {
        if (isInitialized && currentDelimiter == delimiter) {
            // Avoid unnecessary re-initializations.
            return;
        }

        currentDelimiter = delimiter;
        inputFormat = new SimpleDateFormat("ddMMMyyyyhhmmss", Locale.ENGLISH);
        dateFormat = new SimpleDateFormat("dd" + currentDelimiter + "MM"
                + currentDelimiter + "yyyy");
        calendar = Calendar.getInstance();
        isInitialized = true;
    }

    /**
     * Method that provides the formatted date string corresponding to the input
     *
     * @param inputDate
*            String date to format
     * @return the formatted date
     */
    public static String getValue(String inputDate) throws ParseException {
        if (!isInitialized) {
            throw new IllegalStateException(
                    "Date formatter is not initialized");
        }

        Date date = inputFormat.parse(inputDate);
        calendar.setTime(date);
        return dateFormat.format(calendar.getTime());
    }
}
