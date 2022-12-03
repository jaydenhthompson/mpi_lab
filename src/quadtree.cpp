#include "quadtree.hpp"

#include <iostream>

void body::readIn(std::ifstream& in)
{
    in >> this->index;
    in >> this->x;
    in >> this->y;
    in >> this->mass;
    in >> this->vx;
    in >> this->vy;
}

std::tuple<double, double> window::getCenter()
{
    double x = (x_max + x_min) / 2.0;
    double y = (y_max + y_min) / 2.0;
    return std::make_tuple(x, y);
}

node::node()
{
    this->nw = nullptr;
    this->ne = nullptr;
    this->sw = nullptr;
    this->se = nullptr;
}

node::node(body b)
{
    this->type = BODY;
    this->bod = b;
    this->wind = nullptr;
    node();
}

node::node(window* w)
{
    this->type = WINDOW;
    this->bod = body();
    this->wind = w;
    node();
}

quadtree::quadtree(std::vector<body>& init, window* wind)
{
    root = new node(wind);
    for (auto& e : init) {
        addNode(root, e);
    }
    calcCOM(root);
}

void quadtree::addNode(node* cur, body b)
{
    if (!cur)
        return;

    cur->bod.mass += b.mass;
    cur->bod.x += b.x;
    cur->bod.y += b.y;
    double x_cen, y_cen;
    std::tie(x_cen, y_cen) = cur->wind->getCenter();

    node** next = nullptr;
    window* newWind = nullptr;
    auto curWindow = cur->wind;

    auto avgX = [](window* w) { return (w->x_max + w->x_min) / 2.0; };

    auto avgY = [](window* w) { return (w->y_max + w->y_min) / 2.0; };

    if (b.x < x_cen && b.y < y_cen) {
        next = &cur->sw;
        newWind = new window(curWindow->x_min, avgX(curWindow), curWindow->y_min, avgY(curWindow));
    } else if (b.x < x_cen && b.y >= y_cen) {
        next = &cur->nw;
        newWind = new window(curWindow->x_min, avgX(curWindow), avgY(curWindow), curWindow->y_max);
    } else if (b.x >= x_cen && b.y < y_cen) {
        next = &cur->se;
        newWind = new window(avgX(curWindow), curWindow->x_max, curWindow->y_min, avgY(curWindow));
    } else if (b.x >= x_cen && b.y >= y_cen) {
        next = &cur->ne;
        newWind = new window(avgX(curWindow), curWindow->x_max, avgY(curWindow), curWindow->y_max);
    } else {
        std::cerr << "should fit in a quadrant" << std::endl;
        exit(1);
    }

    if (*next) {
        delete newWind;
        if ((*next)->type == BODY) {
            auto tmpBody = (*next)->bod;
            (*next)->type = WINDOW;
            (*next)->bod = body();
            addNode(*next, tmpBody);
        }
        addNode(*next, b);
    } else {
        *next = new node(b);
        (*next)->wind = newWind;
    }
}

std::pair<double, double> quadtree::calcCOM(node* n)
{
    if (!n)
        return std::make_pair(0.0, 0.0);
    auto ne = calcCOM(n->ne);
    auto nw = calcCOM(n->nw);
    auto se = calcCOM(n->se);
    auto sw = calcCOM(n->sw);
    if (n->type == WINDOW) {
        n->bod.x = (ne.first + nw.first + se.first + sw.first) / n->bod.mass;
        n->bod.y = (ne.first + nw.first + se.first + sw.first) / n->bod.mass;
    }
    return std::make_pair(n->bod.x * n->bod.mass, n->bod.y * n->bod.mass);
}
