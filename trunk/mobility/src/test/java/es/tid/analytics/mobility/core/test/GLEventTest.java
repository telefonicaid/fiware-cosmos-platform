package es.tid.analytics.mobility.core.test;

import static org.junit.Assert.assertEquals;

import java.util.Calendar;
import java.util.Date;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import es.tid.analytics.mobility.core.data.GLEvent;

public class GLEventTest {

	GLEvent _glEvent;
	GLEvent _glEventNull;
	Date date;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

	}

	@Before
	public void setUp() throws Exception {
		date = new Date();
	}

	@After
	public void tearDown() throws Exception {
	}

	// GetHour
	@Test
	public void testGetHourOutOfRange() {

		_glEventNull = new GLEvent();

		assertEquals((byte) -1, (byte) _glEventNull.getHour());

	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetHour0() {
		// date.setTime(Date.UTC((2012 - 1900), 1, 30, 0, 58, 45));
		date = new Date((2012 - 1900), 1, 30, 0, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) 0, (byte) _glEvent.getHour());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetHour24() {
		// date.setTime(Date.UTC((2012 - 1900), 1, 30, 24, 58, 45));
		date = new Date((2012 - 1900), 1, 30, 24, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) 0, (byte) _glEvent.getHour());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetHour1to23() {
		// date.setTime(Date.UTC((2012 - 1900), 1, 30, 21, 58, 45));
		date = new Date((2012 - 1900), 1, 30, 21, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) 21, (byte) _glEvent.getHour());
	}

	// GetWeekDay
	@Test
	public void testGetWeekDayNull() {

		_glEventNull = new GLEvent();

		assertEquals((byte) -1, (byte) _glEventNull.getWeekDay());
	}

	/*
	 * Parameters: year - the year minus 1900. month - the month between 0-11.
	 * date - the day of the month between 1-31. hrs - the hours between 0-23.
	 * min - the minutes between 0-59. sec - the seconds between 0-59.
	 */

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayHourChanged() {
		// date.setTime(Date.UTC((2012 - 1900), 2, 30, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 30, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.FRIDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayThursday() {
		// date.setTime(Date.UTC((2012 - 1900), 2, 1, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 1, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.THURSDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayFriday() {

		// date.setTime(Date.UTC((2012 - 1900), 2, 2, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 2, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.FRIDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDaySaturday() {

		// date.setTime(Date.UTC((2012 - 1900), 2, 3, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 3, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.SATURDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDaySunday() {

		// date.setTime(Date.UTC((2012 - 1900), 2, 4, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 4, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.SUNDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayMonday() {

		// date.setTime(Date.UTC((2012 - 1900), 2, 5, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 5, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.MONDAY, (byte) _glEvent.getWeekDay());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayTuesday() {

		// date.setTime(Date.UTC((2012 - 1900), 2, 6, 22, 58, 45));
		date = new Date((2012 - 1900), 2, 6, 22, 58, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, date);

		assertEquals((byte) Calendar.TUESDAY, (byte) _glEvent.getWeekDay());
	}

	@Test
	public void testGetHour0to23Calendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 1, 30, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) 22, (byte) _glEvent.getHour(calendar));
	}

	// GetWeekDay
	@Test
	public void testGetWeekDayNullCalendar() {

		_glEventNull = new GLEvent();

		assertEquals((byte) -1, (byte) _glEventNull.getWeekDay(null));

	}

	@Test
	public void testGetWeekDayWednesdayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 7, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.WEDNESDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

	@Test
	public void testGetWeekDayFridayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 2, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.FRIDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

	@Test
	public void testGetWeekDaySaturdayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 3, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.SATURDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

	@Test
	public void testGetWeekDaySundayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 4, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.SUNDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

	@Test
	public void testGetWeekDayMondayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 5, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.MONDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

	@Test
	public void testGetWeekDayThursdayCalendar() {
		Calendar calendar = Calendar.getInstance();
		calendar.clear();
		calendar.set(2012, 2, 6, 22, 59, 45);
		_glEvent = new GLEvent((long) 1, (long) 2, calendar.getTime());

		assertEquals((byte) Calendar.TUESDAY,
				(byte) _glEvent.getWeekDay(calendar));
	}

}
