#include <fstream>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

struct force {
    double x;
    double y;
    force()
        : x(0)
        , y(0)
    {
    }
    force(double x, double y)
        : x(x)
        , y(y)
    {
    }
    force operator+(const force& a) const
    {
        return force(this->x + a.x, this->y + a.y);
    }
    force& operator+=(const force& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }
};

class body {
public:
    int index;
    double mass;
    double x;
    double y;
    double vx;
    double vy;

    body()
        : index(-1)
        , mass(0)
        , x(0)
        , y(0)
        , vx(0)
        , vy(0)
    {
    }

    void readIn(std::ifstream& in);
};

class window {
public:
    double x_min;
    double x_max;
    double y_min;
    double y_max;

    window(double xmin, double xmax, double ymin, double ymax)
        : x_min(xmin)
        , x_max(xmax)
        , y_min(ymin)
        , y_max(ymax)
    {
    }

    std::tuple<double, double> getCenter();
    double getSideLength()
    {
        return x_max - x_min;
    }
};

enum node_type {
    BODY,
    WINDOW
};

class node {
public:
    node_type type;
    body bod;
    std::shared_ptr<window> wind;
    std::shared_ptr<node> nw;
    std::shared_ptr<node> ne;
    std::shared_ptr<node> sw;
    std::shared_ptr<node> se;

    node();
    node(body);
    node(window);
};

class quadtree {
public:
    quadtree(std::vector<body>&, window);
    void calcBody(body& b, double time, double theta);
    //void update(const std::vector<body>&);

private:
    std::shared_ptr<node> root;
    std::map<int, std::shared_ptr<node>> parents;
    std::pair<double, double> calcCOM(std::shared_ptr<node>, std::shared_ptr<node>);
    force calcForce(std::shared_ptr<node>, const body&, double, double, double);
    std::shared_ptr<node> findParent(std::shared_ptr<node> n, int index);
    void addNode(std::shared_ptr<node>, body);
};