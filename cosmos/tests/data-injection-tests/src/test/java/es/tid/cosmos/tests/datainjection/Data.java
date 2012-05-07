package es.tid.cosmos.tests.datainjection;

import java.util.Iterator;

/**
 *
 * @author ximo
 */
class Data implements Iterable<Integer> {
    private final int step;

    Data(int step) {
        this.step = step;
    }

    @Override
    public Iterator<Integer> iterator() {
        return new Iterator<Integer>() {
            private int item = Byte.MIN_VALUE;

            @Override
            public boolean hasNext() {
                return this.item + Data.this.step > Byte.MAX_VALUE;
            }

            @Override
            public Integer next() {
                this.item += Data.this.step;
                return item;
            }

            @Override
            public void remove() {
                throw new UnsupportedOperationException("Not supported yet.");
            }
        };
    }
}
