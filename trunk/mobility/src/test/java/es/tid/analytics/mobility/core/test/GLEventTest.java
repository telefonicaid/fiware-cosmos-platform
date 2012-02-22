package es.tid.analytics.mobility.core.test;

import static org.junit.Assert.assertEquals;

import java.util.Calendar;

import org.junit.*;

import es.tid.ps.mobility.data.BaseProtocol;
import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MobProtocol.GLEvent;

public class GLEventTest {

	GLEvent.Builder _glEvent;
	GLEvent _glEventNull;
	Date.Builder date;
        Time.Builder time;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {

	}

	@Before
	public void setUp() throws Exception {
		date = Date.newBuilder();
                time = Time.newBuilder();
                _glEvent = GLEvent.newBuilder();
	}

	@After
	public void tearDown() throws Exception {
	}

	// GetHour
	@Test @Ignore
	public void testGetHourOutOfRange() {

                _glEventNull = GLEvent.newBuilder().build();

		assertEquals((byte) -1, (byte) _glEventNull.getTime().getHour());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetHour0() {
            // date = 29/2/2012  00:58:45
                date.setDay(29);
                date.setMonth(2);
                date.setYear(2012);
                date.setWeekday(4);
                time.setHour(0);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) 0, (byte) _glEvent.getTime().getHour());
	}

	@SuppressWarnings("deprecation")
	@Test @Ignore
	public void testGetHour24() {
		// date = 29/2/2012  24:58:45
		date.setDay(29);
                date.setMonth(2);
                date.setYear(2012);
                date.setWeekday(4);
                time.setHour(24);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) 0, (byte) _glEvent.getTime().getHour());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetHour1to23() {
            // date = 29/2/2012  21:58:45
		date.setDay(29);
                date.setMonth(2);
                date.setYear(2012);
                date.setWeekday(4);
                time.setHour(21);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) 21, (byte) _glEvent.getTime().getHour());
	}

	// GetWeekDay
	@Test @Ignore
	public void testGetWeekDayNull() {
            
                _glEventNull = GLEvent.newBuilder().build();

		assertEquals((byte) -1, (byte) _glEventNull.getDate().getWeekday());
	}

	/*
	 * Parameters: year - the year minus 1900. month - the month between 0-11.
	 * date - the day of the month between 1-31. hrs - the hours between 0-23.
	 * min - the minutes between 0-59. sec - the seconds between 0-59.
	 */

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayHourChanged() {
            // date = 29/2/2012  22:58:45
		date.setDay(29);
                date.setMonth(2);
                date.setYear(2012);
                date.setWeekday(4);
                time.setHour(21);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.WEDNESDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayThursday() {
		// date = 01/03/2012  22:58:45
                date.setDay(1);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(5);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.THURSDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayFriday() {

		// date = 02/3/2012  22:58:45
                date.setDay(2);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(6);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.FRIDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDaySaturday() {

                // date = 03/3/2012  22:58:45
                date.setDay(3);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(7);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.SATURDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDaySunday() {

		// date = 04/3/2012  22:58:45
                date.setDay(4);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(1);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.SUNDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayMonday() {

		// date = 05/3/2012  22:58:45
                date.setDay(5);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(2);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.MONDAY, (byte) _glEvent.getDate().getWeekday());
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testGetWeekDayTuesday() {

		// date = 06/3/2012  22:58:45
                date.setDay(6);
                date.setMonth(3);
                date.setYear(2012);
                date.setWeekday(3);
                time.setHour(22);
                time.setMinute(58);
                time.setSeconds(45);
		
		_glEvent.setDate(date);
                _glEvent.setTime(time);

		assertEquals((byte) Calendar.TUESDAY, (byte) _glEvent.getDate().getWeekday());
	}
}
