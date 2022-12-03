#include <fstream>
#include <memory>
#include <tuple>
#include <vector>

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
    //void update(const std::vector<body>&);

private:
    std::shared_ptr<node> root;
    std::pair<double, double> calcCOM(std::shared_ptr<node>);
    body calcBody(body& b, double time);
    void addNode(std::shared_ptr<node>, body);
};