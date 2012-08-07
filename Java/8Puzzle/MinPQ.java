/*************************************************************************
 *  Compilation:  javac MinPQ.java
 *  Execution:    java MinPQ
 *  
 *  Generic min priority queue implementation with a binary heap.
 *
 *  % java MinPQ
 *  a
 *  is
 *  test
 *  this
 *
 *  We use a one-based array to simplify parent and child calculations.
 *
 *************************************************************************/

import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 *  The <tt>MinPQ</tt> class represents a priority queue of generic keys.
 *  It supports the usual <em>insert</em> and <em>delete-the-minimum</em>
 *  operations, along with methods for peeking at the maximum key,
 *  testing if the priority queue is empty, and iterating through
 *  the keys.
 *  <p>
 *  The <em>insert</em> and <em>delete-the-minimum</em> operations are
 *  logarithmic amortized time.
 *  <p>
 *  This implementation uses a binary heap.
 *  <p>
 *  For additional documentation, see <a href="/algs4/34pq">Section 3.4</a> of
 *  <i>Algorithms in Java, 4th Edition</i> by Robert Sedgewick and Kevin Wayne.
 */
public class MinPQ<Key extends Comparable<Key>> implements Iterable<Key> {
    private Key[] pq;     // store element at indices 1 to N
    private int N;        // number of elements on priority queue

   /**
     * Create an empty priority queue with the given initial capacity.
     */
    public MinPQ(int initCapacity) {
        pq = (Key[]) new Comparable[initCapacity + 1];
        N = 0;
    }

   /**
     * Create an empty priority queue.
     */
    public MinPQ() { this(1); }

   /**
     * Is the priority queue empty?
     */
    public boolean isEmpty() {
        return N == 0;
    }

   /**
     * Return the number of elements on the priority queue.
     */
    public int size() {
        return N;
    }

   /**
     * Return the smallest key on the priority queue.
     * Throw an exception if no such key exists because the priority queue is empty.
     */
    public Key min() {
        if (isEmpty()) throw new RuntimeException("Priority queue underflow");
        return pq[1];
    }

    // helper function to double the size of the heap array
    private void resize(int capacity) {
        assert capacity > N;
        Key[] temp = (Key[]) new Comparable[capacity];
        for (int i = 1; i <= N; i++) temp[i] = pq[i];
        pq = temp;
    }

   /**
     * Add a new key to the priority queue.
     */
    public void insert(Key x) {
        // double size of array if necessary
        if (N == pq.length - 1) resize(2 * pq.length);

        // add x, and percolate it up to maintain heap invariant
        pq[++N] = x;
        swim(N);
    }

   /**
     * Delete and return the smallest key on the priority queue.
     * Throw an exception if no such key exists because the priority queue is empty.
     */
    public Key delMin() {
        if (N == 0) throw new RuntimeException("Priority queue underflow");
        exch(1, N);
        Key min = pq[N--];
        sink(1);
        pq[N+1] = null;         // avoid loitering and help with garbage collection
        if ((N > 0) && (N == (pq.length - 1) / 4)) resize(pq.length  / 2);
        return min;
    }


   /***********************************************************************
    * Helper functions to restore the heap invariant.
    **********************************************************************/

    private void swim(int k) {
        while (k > 1 && greater(k/2, k)) {
            exch(k, k/2);
            k = k/2;
        }
    }

    private void sink(int k) {
        while (2*k <= N) {
            int j = 2*k;
            if (j < N && greater(j, j+1)) j++;
            if (!greater(k, j)) break;
            exch(k, j);
            k = j;
        }
    }

   /***********************************************************************
    * Helper functions for comparisons and swaps.
    **********************************************************************/
    private boolean greater(int i, int j) {
        return pq[i].compareTo(pq[j]) > 0;
    }

    private void exch(int i, int j) {
        Key swap = pq[i];
        pq[i] = pq[j];
        pq[j] = swap;
    }

   /**
     * Return an iterator that iterates over all of the keys on the priority queue
     * in ascending order.
     * <p>
     * The iterator doesn't implement <tt>remove()</tt> since it's optional.
     */
    public Iterator<Key> iterator() { return new HeapIterator(); }

    private class HeapIterator implements Iterator<Key> {
        // create a new pq
        private MinPQ<Key> copy = new MinPQ<Key>();

        // add all elements to copy of heap
        // takes linear time since already in heap order so no keys move
        public HeapIterator() {
            for (int i = 1; i <= N; i++)
                copy.insert(pq[i]);
        }

        public boolean hasNext()  { return !copy.isEmpty();                     }
        public void remove()      { throw new UnsupportedOperationException();  }

        public Key next() {
            if (!hasNext()) throw new NoSuchElementException();
            return copy.delMin();
        }
    }

   /**
     * Test client.
     */
    public static void main(String[] args) {
        MinPQ<String> pq = new MinPQ<String>();

        // insert a bunch of strings
        String[] strings = { "it", "was", "the", "best", "of", "times", "it", "was", "the", "worst" };
        for (String s : strings) {
            pq.insert(s);
        }


        // delete and print each key
        while (!pq.isEmpty()) {
            System.out.println(pq.delMin());
        }
        System.out.println();

        // reinsert the same strings
        for (String s : strings) {
            pq.insert(s);
        }
        // print each key using the iterator
        for (String s : pq) {
            System.out.println(s);
        }
        while (!pq.isEmpty()) {
            pq.delMin();
        }
    }

}
