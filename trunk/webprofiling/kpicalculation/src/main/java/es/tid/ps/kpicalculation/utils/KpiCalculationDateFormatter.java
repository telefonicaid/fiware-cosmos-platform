package es.tid.ps.kpicalculation.utils;

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
	private static SimpleDateFormat inputFormat;
	private static SimpleDateFormat dateFormat;
	private static SimpleDateFormat timeFormat;

	private static Calendar calendar;

	private KpiCalculationDateFormatter() {
	}
	
	/**
	 * Method that initializes the formatters
	 */
	public static void init(String delimiter) {
		if (inputFormat != null && dateFormat != null && timeFormat != null
				&& calendar != null) {
			// Avoid unnecessary re-initializations.
			return;
		}
		inputFormat = new SimpleDateFormat("ddMMMyyyyhhmmss", Locale.ENGLISH);
		dateFormat = new SimpleDateFormat("dd" + delimiter + "MM" + delimiter
				+ "yyyy");
		timeFormat = new SimpleDateFormat("HH:mm:ss");
		calendar = Calendar.getInstance();

	}

	/**
	 * Method that provides the formatted date string corresponding to the input
	 * 
	 * @param inputDate
	 *            String date to format
	 * @return the formatted date
	 */
	public static String getValue(String inputDate) throws ParseException {
		Date date = inputFormat.parse(inputDate);
		calendar.setTime(date);
		return dateFormat.format(calendar.getTime());
	}
}
