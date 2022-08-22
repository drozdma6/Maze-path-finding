#include <iostream>
#include <climits>

using namespace::std;

constexpr int WALL = -1;
constexpr int EMPTY = -2;

class Tile
{
public:
    int x;
    int y;
    int dist;
    Tile () = default;
    Tile (int col, int row, int d) : x(col), y(row), dist(d){}
};


class Queue {
public:
    Queue()
    {
        front = rear = nullptr;
    }

    ~Queue()
    {
        Node * cur = front;
        Node * next;
        while (cur)
        {
            next = cur->next;
            delete (cur);
            cur = next;
        }
        front = nullptr;
    }

    void push_back(const Tile & data)
    {
        Node * newNode = new Node(data);
        if (rear == nullptr)
        {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    Tile & Front()
    {
        return front->data;
    }

    void pop()
    {
        if(front == nullptr)
        {
            return;
        }
        Node * cur = front;
        front = front->next;
        if(front == nullptr)
        {
            rear = nullptr;
        }
        delete (cur);
    }

    bool empty()
    {
        return front == nullptr;
    }
private:
    struct Node {
        explicit Node (const Tile & d) : data(d){};
        Node * next = nullptr;
        Tile data;
    };
    Node * front;
    Node * rear;
};


template <typename T> class Vector {
public:
    Vector(){
        vec = new T[1];
        totalCapacity = 1;
        curLen = 0;
    }

    Vector<T> &operator= (const Vector<T> & other)
    {
        if (&other == this)
        {
            return *this;
        }
        delete[] vec;
        curLen = other.size();
        totalCapacity = other.capacity();
        vec = new T[curLen];
        for (size_t i = 0; i < other.size(); i++)
        {
            vec[i] = other.vec[i];
        }
        return *this;
    }

    Vector(const Vector<T> & other)
    {
        curLen = other.size();
        totalCapacity = other.capacity();
        vec = new T[curLen];
        for (size_t i = 0; i < curLen; i++)
        {
            vec[i] = other.vec[i];
        }
    }

    ~Vector(){
        delete[] vec;
    }

    Vector(Vector<T> && other) = delete;
    Vector<T> operator=(Vector<T> && other) = delete;

    void Clear(){
        curLen = 0;
        totalCapacity = 0;
        vec = 0;
    }

    size_t capacity () const
    {
        return totalCapacity;
    }

    void push_back (T data)
    {
        if (curLen == totalCapacity)
        {
            T* newVec = new T[2 * totalCapacity];
            for (size_t i = 0; i < curLen; i++)
            {
                newVec[i] = vec[i];
            }
            totalCapacity *= 2;
            delete[] vec;
            vec = newVec;
        }
        vec[curLen] = data;
        curLen++;
    }

    void pop_back(){
        curLen--;
    }

    size_t size() const {
        return curLen;
    }

    const T & operator[](size_t i) const
    {
        return vec[i];
    }

    T & operator[] (size_t i)
    {
        return vec[i];
    }

    void reverse(){
        int beg = 0;
        int end = curLen;
        while (beg < end)
        {
            T tmp = vec[beg];
            vec[beg] = vec[end - 1];
            vec[end - 1] = tmp;
            beg++;
            end--;
        }
    }

    bool empty() const {
        return curLen == 0;
    }

private:
    T * vec;
    size_t totalCapacity;
    size_t curLen;
};

class Lever
{
public:
    Lever() = default;
    Lever(int id, int dist, const Vector<bool>& configuration) : distance(dist), config(configuration) {
        combinations.push_back(id); //Lever is combination of itself
    };

    void DebugPrintLever() const{
        for (size_t i = 0; i < config.size(); i++)
        {
            cout << config[i];
        }
        cout << endl;
    }

    void CombineWith (Lever other){
        if (combinations.empty())
        {
            for(size_t j = 0; j < other.config.size(); j++)
            {
                config.push_back(false);
            }
        }

        for (size_t i = 0; i < config.size(); i++)
        {
            config[i] = config[i] ^ other.config[i];
        }
        combinations.push_back(other.combinations[0]);
    }

    Vector<int> combinations;
    int distance{};
    Vector<bool> config;
};

class Maze
{
public:
    Maze(size_t na, size_t ka, istream & in) : n(na), k(ka)
    {
        start = Tile(0, n - 1, 0);
        char symbol;
        int distance;
        //Loading levers
        for (size_t i = 0; i < k; i++)
        {
            Vector<bool> conf;
            in >> distance;
            for (size_t j = 0; j < n; j++)
            {
                in >> symbol;
                if (symbol == '0')
                {
                    conf.push_back(false);
                }
                else if (symbol == '1')
                {
                    conf.push_back(true);
                }
            }
            levers.push_back(Lever(i + 1, distance, conf));
        }

        //Loading maze
        visited = new int *[n];
        for (size_t i = 0; i < n; ++i)
        {
            visited[i] = new int[n];
        }

        for (int j = n - 1; j >= 0; j--)
        {
            for (size_t i = 0; i < n; i++)
            {
                in >> symbol;
                if (symbol == '0')
                {
                    visited[j][i] = EMPTY;
                }
                else if (symbol == '1')
                {
                    visited[j][i] = WALL;
                }
            }
        }
        size_t endX, endY;
        in >> endX >> endY;
        end = Tile(endX - 1, n - endY, 0);
        pushedLevers = new bool [k];
        for (size_t i = 0; i < k; ++i)
        {
            pushedLevers[i] = false;
        }
    };

    ~Maze(){
        for(size_t i = 0; i < n; ++i)
        {
            delete[] visited[i];
        }
        delete[] visited;
        delete[] pushedLevers;
        if (shortestPath != -1)
        {
            delete[] path;
        }
    }

    int BFS();
    void ConstructPath();
    void Controller ();
    void ApplyLevers (int ** visited, const Lever & lever) const;
    void CombineLevers(Vector<Lever> & combination, size_t offset, int k, Vector<Lever> & out);

    static Lever Combine (Vector<Lever> levers);

    void PullLevers (bool * curPushedLevers) const; //make all pushed levers equal to 0
    static void PushLevers(const Vector<int> & leversId, bool * activeLevers);
    size_t LeverDistance(const bool * curPushedLevers) const;


    void ResetVisited ();

    void PrintResult() const;
    void PrintLevers() const;
    void PrintPath() const;
    void DebugPrintLevers () const;
    void DebugPrintVisited() const;
    friend ostream & operator << (ostream & out, const Maze & self);

private:
    int ** visited;
    const size_t n; //maze size
    const size_t k; //levers size
    Vector<Lever> levers;
    Tile end{};
    Tile start{};
    int shortestPath = INT_MAX;
    Tile * path = nullptr;
    bool * pushedLevers;
};

int Maze::BFS() {
    if (visited[start.y][start.x] == WALL)
    {
        return -1;
    }
    Queue q; //front
    q.push_back(start);
    visited[start.y][start.x] = 0;

    while (!q.empty())
    {
        Tile cur = q.Front();
        q.pop();
        if (cur.x == end.x && cur.y == end.y)
        {
            end.dist = cur.dist;
            return cur.dist;
        }
        //RIGHT
        if (cur.x + 1 < (int) n && visited[cur.y][cur.x + 1] == EMPTY)
        {
            q.push_back(Tile(cur.x + 1, cur.y, cur.dist + 1));
            visited[cur.y][cur.x + 1] = cur.dist+1;
        }
        //UP
        if (cur.y - 1 >= 0 && visited[cur.y - 1][cur.x] == EMPTY)
        {
            q.push_back (Tile(cur.x, cur.y - 1, cur.dist + 1));
            visited[cur.y - 1][cur.x] = cur.dist+1;
        }
        //DOWN
        if (cur.y + 1 < (int) n && visited[cur.y + 1][cur.x] == EMPTY)
        {
            q.push_back (Tile(cur.x, cur.y + 1, cur.dist + 1));
            visited[cur.y + 1][cur.x] = cur.dist +1;
        }
        //LEFT
        if (cur.x - 1 >= 0 && visited[cur.y][cur.x - 1] == EMPTY)
        {
            q.push_back (Tile(cur.x - 1, cur.y, cur.dist + 1));
            visited[cur.y][cur.x - 1] = cur.dist+1;
        }
    }
    return -1;
}



void Maze::ConstructPath() {
    Tile cur = end;
    if (path != nullptr)
    {
        delete[] path;
    }

    Tile * p = new Tile [end.dist + 1];
    p[end.dist] =end;

    for (int j = end.dist - 1; j >= 0; j--)
    {
        //LEFT
        if (cur.x - 1 >= 0 && visited[cur.y][cur.x - 1] == cur.dist - 1)
        {
            p[j] = Tile(cur.x - 1, cur.y, cur.dist - 1);
        }
            //DOWN
        else if (cur.y + 1 < (int) n && visited[cur.y + 1][cur.x] == cur.dist - 1)
        {
            p[j] = Tile(cur.x, cur.y + 1, cur.dist - 1);
        }
            //RIGHT
        else if (cur.x + 1 < (int) n && visited[cur.y][cur.x + 1] == cur.dist - 1)
        {
            p[j] = Tile(cur.x + 1, cur.y, cur.dist - 1);
        }
            //UP
        else if (cur.y - 1 >= 0 && visited[cur.y - 1][cur.x]== cur.dist - 1)
        {
            p[j] = Tile(cur.x, cur.y - 1, cur.dist - 1);
        }
        cur = p[j];
    }
    path = p;
}

void Maze::ApplyLevers(int** visited, const Lever & lever) const {
    for (size_t j = 0; j < n; j++)
    {
        for (size_t k = 0; k < n; k++)
        {
            if (lever.config[k])
            {
                if (visited[j][k] == WALL)
                {
                    visited[j][k] = EMPTY;
                }
                else{
                    visited[j][k] = WALL;
                }
            }
        }
    }
}

Lever Maze::Combine (Vector<Lever> levers)
{
    Lever l;
    for (size_t i = 0; i < levers.size(); i++)
    {
        l.CombineWith(levers[i]);
    }
    return l;
}

void Maze::CombineLevers(Vector<Lever> & combination, size_t offset, int k, Vector<Lever> & out) {
    if (k == 0) {
        out.push_back(Combine(combination));
        return;
    }
    for (size_t i = offset; i <= levers.size() - k; ++i) {
        combination.push_back(levers[i]);
        CombineLevers(combination,i+1, k-1, out);
        combination.pop_back();
    }
}

void Maze::Controller() {
    int curBFS;
    bool* curPushedLevers = new bool[k];

    curBFS = BFS();
    shortestPath = curBFS;
    if (curBFS != -1)
    {
        ConstructPath();
    }
    //generate combinations of levers
    for (size_t i = 1; i <= k; i++)
    {
        Vector<Lever> combinatedLevers, vec;
        CombineLevers(vec, 0, i, combinatedLevers);

        for (size_t j = 0; j < combinatedLevers.size(); j++)
        {
            ResetVisited();

            ApplyLevers(visited, combinatedLevers[j]);


            curBFS = BFS();
            if (curBFS != -1)
            {
                PullLevers(curPushedLevers);
                PushLevers(combinatedLevers[j].combinations, curPushedLevers);
                curBFS += LeverDistance(curPushedLevers);
                if(shortestPath == -1 || curBFS < shortestPath)
                {
                    for (size_t l = 0; l < k; l++)
                    {
                        pushedLevers[l] = curPushedLevers[l];
                    }
                    shortestPath = curBFS;
                    ConstructPath();
                }
            }
            ApplyLevers(visited, combinatedLevers[j]);
        }
    }
    delete[] curPushedLevers;
    PrintResult();
}


void Maze::PrintPath() const {
    for (int i = 0; i < end.dist; i++)
    {
        cout << '[' << path[i].x + 1 << ';' << n - path[i].y << ']' << ',';
    }
    cout << '[' << path[end.dist].x + 1 << ';' << n - path[end.dist].y << ']' << endl;
}


void Maze::PullLevers(bool * curPushedLevers) const {
    for (size_t i = 0; i < k; i++)
    {
        curPushedLevers[i] = false;
    }
}

void Maze::PushLevers(const Vector<int> & leversId, bool * activeLevers ) {
    for(size_t i = 0; i < leversId.size(); i++)
    {
        activeLevers[leversId[i] - 1] = true; //ids start at 1
    }
}

void Maze::PrintLevers() const{
    if (k == 0)
    {
        return;
    }
    for(size_t j = 0; j < k; j++)
    {
        cout << pushedLevers[j];
    }
    cout << endl;
}

size_t Maze::LeverDistance(const bool * curPushedLevers) const {
    int furthestLever = 0, dist = 0;

    for (size_t i = 0; i < k; i++)
    {
        if (!curPushedLevers[i])
        {
            continue;
        }
        if (levers[i].distance == - 1)
        {
            dist += 1;
        }
        else if (levers[i].distance > furthestLever)
        {
            furthestLever = levers[i].distance;
        }
    }
    dist += furthestLever;
    return 2 * dist;
}

void Maze::PrintResult() const{
    if (shortestPath == -1)
    {
        cout << "-1" << endl;
        return;
    }
    cout << shortestPath << endl;
    PrintLevers();
    PrintPath();
}



void Maze::DebugPrintLevers() const {
    for (size_t i = 0; i < k; i++)
    {
        levers[i].DebugPrintLever();
    }
}

void Maze::ResetVisited() {
    for (size_t i = 0; i < n; i++)
    {
        for(size_t j = 0; j < n; j++)
        {
            if (visited[i][j] != WALL){
                visited[i][j] = EMPTY;
            }
        }
    }
}

void Maze::DebugPrintVisited() const {
    cout << "----------------------------------------" << endl;

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (visited[i][j] == EMPTY)
            {
                cout << '0';
            }
            else if (visited[i][j] == WALL)
            {
                cout << '#';
            }
            else{
                cout << visited[i][j];
            }
        }
        cout << endl;
    }
    cout << "----------------------------------------" << endl;
}

int main()
{
    size_t n, k;
    cin >> n >> k;
    Maze map (n,k, cin);
    map.Controller();
    return 0;
}