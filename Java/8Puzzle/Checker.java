/*************************************************************************
 *  Compilation:  javac Checker.java
 *  Execution:    java Checker filename1.txt filename2.txt ...
 *  Dependencies: Board.java Solver.java In.java
 *
 *  This program creates an initial board from each filename specified
 *  on the command line and finds the minimum number of moves to
 *  reach the goal state.
 *
 *  % java Checker puzzle*.txt
 *  puzzle00.txt: 0
 *  puzzle01.txt: 1
 *  puzzle02.txt: 2
 *  puzzle03.txt: 3
 *  puzzle04.txt: 4
 *  puzzle05.txt: 5
 *  puzzle06.txt: 6
 *  ...
 *  puzzle3x3-impossible: -1
 *  ...
 *  puzzle42.txt: 42
 *  puzzle43.txt: 43
 *  puzzle44.txt: 44
 *  puzzle45.txt: 45
 *
 * 
 *  Remark: In.java is part of stdlib.jar. You can also download
 *  directly from http://www.cs.princeton.edu/introcs/stdlib/
 *
 *************************************************************************/

public class Checker {

    public static void main(String[] args) {

        for (String filename : args) {
            In in = new In(filename);
            int N = in.readInt();
            int[][] tiles = new int[N][N];
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    tiles[i][j] = in.readInt();
                }
            }
            Board board = new Board(tiles);
            Solver solver = new Solver(board);
            System.out.println(filename + ": " + solver.moves());
        }
    }

}
