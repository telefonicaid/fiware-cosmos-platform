package es.tid.analytics.mobility.core.data;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;

public class Utils {

	public static long convertAscii2Decimal(final String text) {
		// TODO
		return (long) 0;
	}


	public static ArrayList<Double> normalizationOfVector(
			final ArrayList<Double> rawVector) {
		// Output
		final ArrayList<Double> normVector = new ArrayList<Double>();
		// Intermediate
		final ArrayList<Double> tempVector = new ArrayList<Double>();
		double sum = 0;

		Iterator<Double> iter = rawVector.iterator();
		// Step 1: Normalized by number of days of group
		for (int pos = 0; iter.hasNext(); pos++) {
			if (pos < 24) {
				tempVector.add(iter.next() / 102.0); //L-v:102
			} else {
				tempVector.add(iter.next() / 26.0); // V:26, S:26, D:26
			}
			sum += tempVector.get(pos);
		}
		// Step 2: Normalized by sum of values
		iter = tempVector.iterator();
		while (iter.hasNext()) {
			normVector.add(iter.next() / sum );
		}

		return normVector;
	}

	public static int getPosVectorComunications(final byte weekDay,
			final byte hour) {
		final int groupday;

		switch (weekDay) {
		case Calendar.FRIDAY:
			// TODO Definir constantes para groupday
			groupday = 1;
			break;

		case Calendar.SATURDAY:
			groupday = 2;
			break;

		case Calendar.SUNDAY:
			groupday = 3;
			break;

		default:
			groupday = 0;
			break;
		}

		return groupday * 24 + hour;
	}
	
	public static int getPosListNoInfoGeolocations(final byte weekD, final byte h) {
		// hope weekD: from Calendar.SUNDAY to Calendar.SATURDAY
		// hope h: from 0 to 23
		int position = -1;

		if (h >= 0 && h <= 23 && weekD <= Calendar.SATURDAY
				&& weekD >= Calendar.SUNDAY) {
			// position: from 0 to (TAMLIST-1)
			position = (h * 7) - 1 + weekD;
		}
		// Return -1 by default
		return position;
	}
	
	
	public static int getPosAddNoInfoGeolocations(final int pos) {
		int position = 0, groupday = 0;
		int hour = 0;

		hour = (pos / 7);
		position = pos - (hour * 7 - 1);

		switch (position) {
		case Calendar.FRIDAY:
			groupday = 1;
			break;

		case Calendar.SATURDAY:
			groupday = 2;
			break;

		case Calendar.SUNDAY:
			groupday = 3;
			break;

		default:
			groupday = 0;
			break;
		}
		return groupday * 24 + hour;
	}
}
