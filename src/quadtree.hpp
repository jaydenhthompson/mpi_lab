#include <vector>
#include <tuple>

class body
{
public:
    int bodyIndex;
    double mass;
    double x;
    double y;
    double vx;
    double vy;

    body()
        : bodyIndex(-1),
          mass(0),
          x(0),
          y(0),
          vx(0),
          vy(0) {}
};

class window
{
public:
    double x_min;
    double x_max;
    double y_min;
    double y_max;

    window(double xmin, double xmax, double ymin, double ymax)
        : x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax) {}

    std::tuple<double, double> getCenter();
};

enum node_type {
    BODY,
    WINDOW
};

class node
{
public:
    node_type type;
    body *self_body;
    window *self_window;
    node *nw;
    node *ne;
    node *sw;
    node *se;

    node();
    node(body*);
    node(window*);
};

class quadtree
{
public:
    quadtree(std::vector<body*>, window*);
private:
    node *root;
    void addNode(body*);
};