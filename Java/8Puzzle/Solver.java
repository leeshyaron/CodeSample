/*---------------------------------------------------------------------------
 *  Author:        Mark Xia & Wenxin Zhu
 *  Precept:       03
 *  Written:       3/03/09
 *
 *  Compilation:   javac Solver.java
 *  Execution:     java Solver.java < puzzle01.txt
 *  
 *  Solver is an object class that is designed to solve 8puzzles and puzzles
 *  of the same form.
 * 
 *  Its API includes the following:
 * 
 *     public Solver(Board initial)           //  Finds a solution to initial
 *                                                if it exists.
 *     public boolean isSolvable()            //  Returns true if initial board
 *                                                is solvable, and false 
 *                                                otherwise.
 *     public int moves()                     //  Returns minimum number of 
 *                                                moves to solve the initial
 *                                                board. Return -1 if no
 *                                                such solution.
 *     public String toString()               //  Returns string representation
 *                                                of solution.
 *     public static void main(String[] args) //  Read puzzle instance from 
 *                                                stdin and print solution to
 *                                                stdout, with the number of 
 *                                                states enqueued and number of
 *                                                moves. Prints "No solution 
 *                                                possible" if puzzle is not
 *                                                solvable.
 * 
 * % java Solver < puzzle04.txt
 *     1  3 
 *  4  2  5 
 *  7  8  6 
 * 
 *  1     3 
 *  4  2  5 
 *  7  8  6 
 * 
 *  1  2  3 
 *  4     5 
 *  7  8  6 
 * 
 *  1  2  3 
 *  4  5   
 *  7  8  6 
 * 
 *  1  2  3 
 *  4  5 6 
 *  7  8   
 * 
 * Number of states enqueued = 10
 * Number of moves = 4
 * ---------------------------------------------------------------------------*/

public class Solver {
  private final Board initial;                  // initial board
  private final State goal;                     // goal state
  private final int count;                      // number of enqueues
  
  // find a solution to the initial board
  public Solver(Board initial)   
  {
    MinPQ<State> queue = new MinPQ<State>();    // priority queue used to solve
    State st = new State(initial, 0, null);     // initial state
    int numEnqueue = 0;                         // number of enqueues
    this.initial = initial;                     // initial board
    
    if(isSolvable())
    {
      if(initial.manhattan() == 0)
      {
        goal = st;
        count = numEnqueue;
      }
      else
      {
        queue.insert(st);
        numEnqueue++;
        while(true)
        {     
          st = queue.delMin();
          Board b = st.b;
          if(b.manhattan() == 0)
            break;
          for(Board x: b.neighbors())
            if(st.prev == null || !(x.equals((st.prev).b)))
            {
              State neighbor = new State(x, st.moves + 1, st);
              queue.insert(neighbor);
              numEnqueue++;
            }
        }
        goal = st;
        count = numEnqueue;
      }
    }
    else
    {
      goal = null;
      count = 0;
    }
  }
  
  // Returns true if initial board is solvable, and false otherwise.
  public boolean isSolvable()    
  {
    int inv = 0;                         // number of inversions
    int tiles[][] = initial.getTiles();  // board of initial
    int N = initial.getN();              // row length and col length of initial
    int row = initial.getRow();          // row where 0 (blank space) is in
    
    for(int i = 0; i < N; i++)
      for(int j = 0; j < N; j++)
        if(tiles[i][j] != 0)
        {
          int current = tiles[i][j];     /* current element to count
                                            inversions based on. */
          for(int y = j; y < N; y++)
            if(tiles[i][y] != 0 && tiles[i][y] < current)
              inv++;     
          for(int x = i + 1; x < N; x++)
            for(int y = 0; y < N; y++)
              if(tiles[x][y] != 0 && tiles[x][y] < current)
                inv++;
        }
    if(N % 2 == 1 && inv % 2 == 0)
      return true;
    if(N % 2 == 0 && row % 2 == 0 && inv % 2 == 1)
      return true;
    if(N % 2 == 0 && row % 2 == 1 && inv % 2 == 0)
      return true;
    return false;
  } 
  
  /* return minimum number of moves to solve the initial board. Return -1 if no
     such solution */
  public int moves()             
  {
    if(!isSolvable())
      return -1;
    return goal.moves;
  }
  
  // Return string representation of solution
  public String toString()       
  {
    String s = "";
    State st = goal;
    while(true)
    {
      s = st.b + "\n" + s;
      if(st.prev != null)
        st = st.prev;
      else
        return s;
    }
  }
  
  /* State represents the state of the game. Includes board, number of moves to
     reach it, and previous state. */
  private class State implements Comparable<State>
  {
    private final Board b;                // board
    private final int moves;              // number of moves to reach this board
    private final State prev;             // previous state
    
    public State(Board b, int moves, State prev)
    {
      this.b = b;
      this.moves = moves;
      this.prev = prev;
    }
    
    /* Compare Manhattan function of this State and that. 
       Return -1 if this State's manhattan is lower.
       Return +1 if this State's manhattan is higher.
       Return 0 if this State's manhattan is equal to that's manhattan.
       Can also compare via Hamming function if "true" is set to "false". */
    public int compareTo(State that)
    {
      if(true)
      {
        if(b.manhattan() + moves < (that.b).manhattan() + that.moves)
          return -1;
        if(b.manhattan() + moves > (that.b).manhattan() + that.moves)
          return +1;
        return 0;
      }
      else
      {
        if(b.hamming() + moves < (that.b).hamming() + that.moves)
          return -1;
        if(b.hamming() + moves > (that.b).hamming() + that.moves)
          return +1;
        return 0;
      }
    }
  }
  
  /* Read puzzle instance from stdin and print solution to stdout, with the
     number of states enqueued and number of moves. Prints "No solution 
     possible" if puzzle is not solvable. */
  public static void main(String[] args)
  {
    int N = StdIn.readInt();               // row length of board
    int[][] tiles = new int[N][N];         // board's tiles
    for (int i = 0; i < N; i++)
      for (int j = 0; j < N; j++)
        tiles[i][j] = StdIn.readInt();
    Board board = new Board(tiles);        // board
    Solver sol = new Solver(board);        // solver for board
    if(sol.isSolvable())
    {
      System.out.print(sol);
      System.out.println("Number of states enqueued = " + sol.count);
      System.out.println("Number of moves = " + (sol.goal).moves);
    }
    else
      System.out.println("No solution possible");
  }
}
