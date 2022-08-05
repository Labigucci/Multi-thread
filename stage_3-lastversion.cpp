#include <thread>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <ncurses.h>
#include <mutex>
#include <condition_variable>
//Task description:
//First stage: in the first stage I had to have 4 letters which go back and forth with different speeds.
//Second stage: For the second stage I had to have a critical section in which there can be only one letter from each side
//If characters approach from one side the stop, but a character from the other side can proceed. Process occurs independently.
//Third task:
//5th ball flying around back and forth and when the ball will be in the critical area, no ball can enter until that ball leaves while all the other rules are kept,
// if the others are inside they can still move.

/* Changes: Added a lock_guard to the condition of incrementing and decrementing. We add it cause without it the race condition is not synchronised
Condition variable notifies ecerytime the O point changes.
*/

using namespace std;
bool ready = true;
mutex m;
mutex m1;
mutex m2;

std::condition_variable  cv;
int A=19,B=19,C=0,D=0,O;


void ball(int &point, int speed)
{
    bool incrementing = true;
    while(ready)
    {
        if(incrementing)
        {

            point ++;
        }

        if(!incrementing)
        {

            point --;
        }

        cv.notify_all();
        this_thread::sleep_for(chrono::milliseconds(speed));

        if(point == 20 ||  point == 0)// Length of the line is 20
            incrementing = !incrementing;
    }
}

void worker(int &point, int speed)
{
    bool incrementing = true;
    while(ready)
    {
        if(incrementing)
        {
            point ++;
        }

        if(!incrementing)
        {
            point --;
        }

        this_thread::sleep_for(chrono::milliseconds(speed));
        if(point == 20 ||  point == 0)// Length of the line is 20
            incrementing = !incrementing;


        if(point > 5 && point < 15)
        {

            if (incrementing)
            {
                unique_lock<mutex> lock (m); //Lock for the first limit
                {

                    cv.wait(lock, [] {return O <= 5 || O >= 15;} ); // O is the point of the ball
                }
                while(point > 5 && point < 15)
                {
                    point ++;
                    this_thread::sleep_for(chrono::milliseconds(speed));
                }
            }
            else
            {
                unique_lock<mutex> locki (m1);//Lock for the second limit
                {

                    cv.wait(locki, [] {return O <= 5 || O >= 15;} );// O is the point of the ball
                }
                while(point > 5 && point < 15)
                {
                    point --;
                    this_thread::sleep_for(chrono::milliseconds(speed));
                }
            }
        }

    }
}







void Printing()
{
    while(ready)
    {
        this_thread::sleep_for(chrono::milliseconds(20));
        erase();

        mvaddch(0,A,'A');
        mvaddch(1,B,'B');
        mvaddch(2,C,'C');
        mvaddch(3,D,'D');
        mvaddch(4,O,'O');
        refresh();
        mvprintw(5,0,"------|-------|------");
        //for(int i = 0; i <= 3; i++){
        //mvprintw(i,6,"|    |");

        refresh();
    }


}



int main()
{
    initscr();
    thread thread1{[]{worker(A,500);}};
    thread thread2{[]{worker(B,500);}};
    thread thread3{[]{worker(C,500);}};
    thread thread4{[]{worker(D,500);}};
    thread threadball{[]{ball(O,300);}};
    thread thread5= thread(&Printing);

    if(getch())
    {
        ready = false;
        thread1.join();
        thread2.join();
        thread3.join();
        thread4.join();
        threadball.join();
        thread5.join();
    }
    endwin();
    return 0;
}


