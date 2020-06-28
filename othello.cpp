#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits.h>



struct Point 
{
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
std::vector<std::vector<int>> statebonus=
{{  65,  -3,  6, 4, 4,  6,  -3,  65},
{   -3, -29,  3, 1, 1, -3, -29,  -3},
{    6,   3,  5, 3, 3,  5,   3,   6},
{    4,   1,  3, 1, 1,  3,   1,   4},
{    4,   1,  3, 1, 1,  3,   1,   4},
{    6,   3,  5, 3, 3,  5,   3,   6},
{   -3, -29,  3, 1, 1, -3, -29,  -3},
{   65,  -3,  6, 4, 4,  6,  -3,  65}};

std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
const int SIZE = 8;
std::array <std::array <int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;


int min(std::array <std::array <int, SIZE>, SIZE> curboard,int cur_player,int depth,int alpha,int beta);
int max(std::array <std::array <int, SIZE>, SIZE> curboard,int cur_player,int depth,int alpha,int beta);
//main.cpp
class state 
{
public:
    enum SPOT_STATE
    {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    int cur_player;

public:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const 
    {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    state() 
    {
        reset();
    }
    void reset() 
    {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        next_valid_spots = get_valid_spots();
    }
    std::vector<Point> get_valid_spots() const 
    {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) 
        {
            for (int j = 0; j < SIZE; j++) 
            {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) 
    {
        if(!is_spot_valid(p)) {
            return false;
        }
        set_disc(p, cur_player);
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) 
            {
                // Game ends

            }
        }
        return true;
    }
    
};


//player.cpp
void read_board(std::ifstream& fin) 
{
    fin >> player;
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            fin >> board[i][j];
        }
    }
}

int getvalue(Point x,std::array <std::array <int, SIZE>, SIZE> curboard,int cur_player)
{
    int value=0;
    state cur;
    cur.board=curboard;
    value+=statebonus[x.x][x.y];
    for (Point dir: directions)
        {
            int temp=0;
            Point p = x + dir;
            if (!cur.is_disc_at(p, cur.get_next_player(cur_player)))
                continue;
            p = p + dir;
            value+=5;
            while (cur.is_spot_on_board(p) && cur.get_disc(p) != cur.EMPTY) 
            {
                temp+=5;
                if (cur.is_disc_at(p, cur_player))
                {
                    value+=temp;
                }
                p = p + dir;
            }
        }
    return value;
}


int max(std::array <std::array <int, SIZE>, SIZE> curboard,int cur_player,int depth,int alpha,int beta)
{
    state cur;
    cur.board=curboard;
    cur.cur_player=cur_player;
    cur.next_valid_spots=cur.get_valid_spots();
    int n_spot=cur.next_valid_spots.size();
    std::vector<int> spotvalue(n_spot);
    int maxindex=0;
    int value=INT_MIN;
    for(int i=0;i!=n_spot;i++)
    {
        spotvalue[i]=getvalue(cur.next_valid_spots[i],cur.board,cur.cur_player);
    }
    if(depth<3)
    {
        int bestval=INT_MIN;
        for(int i=0;i!=n_spot;i++)
        {
            state next;
            next.board=cur.board;
            next.cur_player=next.get_next_player(cur.cur_player);
            next.set_disc(cur.next_valid_spots[i],cur.cur_player);
            next.flip_discs(cur.next_valid_spots[i]);
            next.next_valid_spots=next.get_valid_spots();
            if(next.next_valid_spots.size()==0)
            {
                continue;
            }
            int index=0;
            index=min(next.board,next.cur_player,depth+1,alpha,beta);
            spotvalue[i]+=getvalue(next.next_valid_spots[index],next.board,next.cur_player);
        }
    }

    for(int i=0;i!=n_spot;i++)
    {
        if(value<spotvalue[i])
        {
            value=spotvalue[i];
            maxindex=i;
        }
    }
    return maxindex;
}


int min(std::array <std::array <int, SIZE>, SIZE> curboard,int cur_player,int depth,int alpha,int beta)
{
    state cur;
    cur.board=curboard;
    cur.cur_player=cur_player;
    cur.next_valid_spots=cur.get_valid_spots();
    int n_spot=cur.next_valid_spots.size();
    std::vector<int> spotvalue(n_spot);
    int minindex=0;
    int value=INT_MAX;
    for(int i=0;i!=n_spot;i++)
    {
        spotvalue[i]=getvalue(cur.next_valid_spots[i],cur.board,cur.cur_player);
    }

    if(depth<3)
    {
        for(int i=0;i!=n_spot;i++)
        {
            state next;
            next.board=cur.board;
            next.cur_player=next.get_next_player(cur.cur_player);
            next.set_disc(cur.next_valid_spots[i],cur.cur_player);
            next.flip_discs(cur.next_valid_spots[i]);
            next.next_valid_spots=next.get_valid_spots();
            if(next.next_valid_spots.size()==0)
            {
                continue;
            }
            int index=0;
            index=max(next.board,next.cur_player,depth+1,alpha,beta);
            spotvalue[i]+=getvalue(next.next_valid_spots[index],next.board,next.cur_player);
        }
    }

    for(int i=0;i!=n_spot;i++)
    {
        if(value>spotvalue[i])
        {
            value=spotvalue[i];
            minindex=i;
        }
    }
    return minindex;
}



void write_valid_spot(std::ofstream& fout)
{
    srand(time(NULL));
    state curboard;
    curboard.board=board;
    curboard.cur_player=player;
    curboard.next_valid_spots=curboard.get_valid_spots();
    int index=0;
    index=max(curboard.board,curboard.cur_player,0,INT_MIN,INT_MAX);
    Point p = curboard.next_valid_spots[index];
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    //read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

