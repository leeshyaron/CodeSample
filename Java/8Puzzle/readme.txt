/**********************************************************************
 *  8-Puzzle readme.txt template
 **********************************************************************/

Name:             Mark Xia
Login:            mtxia
Precept:          03
Partner name:     Wenxin Zhu
Partner login:    wkzhu
Partner precept:  03
Hours to complete assignment (optional):  ~10 hours

/**********************************************************************
 *  Explain briefly how you implemented Board.java.
    What methods (if any) did you add to the Board API? Why?
 **********************************************************************/
I used an int[][] called "tiles" to represent the board, with elements
corresponding to the numbers in the board. 0 represented a blank space.
The constructor creates a new "tiles" and copies the argument's elements
into tiles. I also have private variables row and col to mark where the 
blank space is at. They are used for toString() and neighbors(). The private
variable N marks the length/width of the board. hamming() and manhattan()
were implemented as the instructions told us to. equals() is implemented
throughly by checking all trivial cases first to avoid having to check 
each element unless necessary. neighbors() was done by adding neighbors
into a stack and returning the stack. toString() is implemented such that
the format still retains, even for N larger (as well as smaller) than 3.
  
I added some accessor methods so I could access private variables in
Board from Solver (getTiles(), getN(), getRow(), getCol()).
/**********************************************************************
 *  Explain briefly how you represented states in the game
*   (board + number of moves + previous state).
 **********************************************************************/
I represented it as a private class within Solver.java. The constructor
takes arguments board, number of moves, and previous state and assigns
them to the private variables (Board board, int moves, State prev) within
the class. The compareTo() just compares the manhattan() + number of moves
for each of the 2 states and returns -1, +1, or 0 if the current state's
manhattan() + number of moves is smaller, greater, or equal to the argument
state's manhattan() + number of moves respectively. The else clause is for
hamming(), so you can just put in false in if(true) to use hamming().

/**********************************************************************
 *  Explain briefly how you detected unsolvable puzzles.
 **********************************************************************/
I used a mathematical formula involving two steps. First, I count the
number of "inversions," which are defined to be the number of elements
to the right or below some element x, that are smaller than x. Then I
checked three conditions using the number of inversions to determine 
solvability.

  1) If the board's row and column lengths, N, was odd and the number of
     inversions was even, then the puzzle was solvable.
  2) If the board's row and column lengths, N, was even, and the row of
     the blank space was even, and the number of inversions was odd, then
     the puzzle was solvable.
  3) If the board's row and column lengths, N, was even, and the row of
     the blank space was odd, and the number of inversions was even, then
     the puzzle was solvable.

Otherwise, the puzzle was not solvable.
  
The reasoning behind this formula comes from the idea that there are 0
inversions at the goal state, and that each up/down move, when N is odd, will
decrease or increase the number of inversions by an even amount each time.
Note also each left/down move will not change the inversions, so this case
is trivial. Hence, to reach 0 inversions at the end and get solvability, the 
initial number of inversions must be even when N is odd 
(because odd - even != even != 0). When N is even, it is slightly different
but essentially the same reasoning. The details can be found at
http://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html

/**********************************************************************
 *  For each of the following instances, give the minimal number of 
 *  moves to reach to goal state. Also give the number of states
 *  enqueued using the A* heuristic with the Hamming and Manhattan
 *  priority functions.
 **********************************************************************/

                  number of       states enqueued
     instance       moves      Hamming     Manhattan
   ------------  ----------   ----------   ----------
   puzzle28.txt      28         634975        16730
   puzzle30.txt      30        1249898        22390
   puzzle32.txt      32       Can't solve    503454
   puzzle34.txt      34       Can't solve    154469
   puzzle36.txt      36       Can't solve   2014323
   puzzle38.txt      38       Can't solve    556106
   puzzle40.txt      40       Can't solve    598398
   puzzle42.txt      42       Can't solve   2850357

/**********************************************************************
 *  If you wanted to solve random 4-by-4 or 5-by-5 puzzles, which
 *  would you prefer:  more time (say 10x as much), more memory (say
 *  (10x as much), or a better priority function.
 **********************************************************************/
 A better priority function because the additional efficiency will 
 reduce the amount of memory and time needed. So in a sense you are also
 getting more time and memory by choosing a better priority function.
 For example, using a "pattern database" as described in the Checklist's
 Enrichment section would make the solution better.

/**********************************************************************
 *  Known bugs / limitations.
 **********************************************************************/
The hamming() function can't seem to solve some puzzles 32 and up. I
consistently run out of memory each time. However, the manhattan()
can still solve them.

/**********************************************************************
 *  List whatever help (if any) that you received. Be specific with
 *  the names of lab TAs, classmates, or course staff members.
 **********************************************************************/
 My partner and I did not collaborate with anyone. All of this code is 
 written by us through pair programming. The only help we received was 
 from our preceptor. He helped us debug a very dumb error I made (returning
 -1 instead of +1 in compareTo() and writing moves intead of
 that.moves for the second half of the comparison), which was extremely
 helpful since our program was running out of memory for puzzle04.txt
 due to the bug! 

/**********************************************************************
 *  Describe any serious problems you encountered.                    
 **********************************************************************/
 Aside from the memory bug coming from compareTo(), there were no
 serious problems.

/**********************************************************************
 *  List any other comments here. Feel free to provide any feedback   
 *  on how much you learned from doing the assignment, and whether    
 *  you enjoyed doing it.                                             
 **********************************************************************/
  
  This assignment was a very interesting one because the 8 Puzzle (and
  puzzles of this type in general) is a pretty cool puzzle. I enjoyed
  learning the A* algorithm to solve this. I like how this assignment
  demonstrates some problems in real life cannot be solved without
  programming knowledge. It's always nice to have very cool and realistic
  applications from what you are learning, which doesn't happen too often
  in other subjects...
