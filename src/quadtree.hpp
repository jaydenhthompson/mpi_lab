#include <vector>
#include <tuple>
#include <fstream>

class body
{
public:
    int index;
    double mass;
    double x;
    double y;
    double vx;
    double vy;

    body()
        : index(-1),
          mass(0),
          x(0),
          y(0),
          vx(0),
          vy(0) {}

    void readIn(std::ifstream &in);
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

enum node_type
{
    BODY,
    WINDOW
};

class node
{
public:
    node_type type;
    body bod;
    window *wind;
    node *nw;
    node *ne;
    node *sw;
    node *se;

    node();
    node(body);
    node(window*);
    ~node() {
        delete this->wind;
    }
};

class quadtree
{
public:
    quadtree(std::vector<body>&, window *);
    ~quadtree()
    {
        this->del(root);
    }
private:
    node *root;
    void addNode(node *, body);
    std::pair<double, double> calcCOM(node *);
    void del(node *n)
    {
        if(!n) return;
        del(n->nw);
        del(n->ne);
        del(n->se);
        del(n->sw);
        delete n;
    }
};