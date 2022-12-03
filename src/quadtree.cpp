#include "quadtree.hpp"

#include <cmath>
#include <iostream>
#include <mpi.h>

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

node::node(window w)
{
    this->type = WINDOW;
    this->bod = body();
    this->wind = std::make_shared<window>(w);
    node();
}

quadtree::quadtree(std::vector<body>& init, window wind)
{
    root = std::make_shared<node>(wind);
    for (auto& e : init) {
        if (e.mass < 0)
            continue;
        addNode(root, e);
    }
    calcCOM(root, nullptr);
}

void quadtree::addNode(std::shared_ptr<node> cur, body b)
{
    if (!cur)
        return;

    cur->bod.mass += b.mass;
    double x_cen, y_cen;
    std::tie(x_cen, y_cen) = cur->wind->getCenter();

    std::shared_ptr<node>* next = nullptr;
    std::shared_ptr<window> newWind = nullptr;
    auto curWindow = cur->wind;

    auto avgX = [](std::shared_ptr<window> w) { return (w->x_max + w->x_min) / 2.0; };

    auto avgY = [](std::shared_ptr<window> w) { return (w->y_max + w->y_min) / 2.0; };

    if (b.x < x_cen && b.y < y_cen) {
        next = &cur->sw;
        newWind = std::make_shared<window>(curWindow->x_min, avgX(curWindow), curWindow->y_min, avgY(curWindow));
    } else if (b.x < x_cen && b.y >= y_cen) {
        next = &cur->nw;
        newWind = std::make_shared<window>(curWindow->x_min, avgX(curWindow), avgY(curWindow), curWindow->y_max);
    } else if (b.x >= x_cen && b.y < y_cen) {
        next = &cur->se;
        newWind = std::make_shared<window>(avgX(curWindow), curWindow->x_max, curWindow->y_min, avgY(curWindow));
    } else if (b.x >= x_cen && b.y >= y_cen) {
        next = &cur->ne;
        newWind = std::make_shared<window>(avgX(curWindow), curWindow->x_max, avgY(curWindow), curWindow->y_max);
    } else {
        std::cerr << "should fit in a quadrant" << std::endl;
        exit(1);
    }

    if (*next) {
        if ((*next)->type == BODY) {
            auto tmpBody = (*next)->bod;
            (*next)->type = WINDOW;
            (*next)->bod = body();
            addNode(*next, tmpBody);
        }
        addNode(*next, b);
    } else {
        *next = std::make_shared<node>(b);
        (*next)->wind = newWind;
    }
}

std::pair<double, double> quadtree::calcCOM(std::shared_ptr<node> n, std::shared_ptr<node> p)
{
    if (!n)
        return std::make_pair(0.0, 0.0);
    auto ne = calcCOM(n->ne, n);
    auto nw = calcCOM(n->nw, n);
    auto se = calcCOM(n->se, n);
    auto sw = calcCOM(n->sw, n);
    if (n->type == WINDOW) {
        n->bod.x = (ne.first + nw.first + se.first + sw.first) / n->bod.mass;
        n->bod.y = (ne.first + nw.first + se.first + sw.first) / n->bod.mass;
    } else {
        parents[n->bod.index] = p;
    }
    return std::make_pair(n->bod.x * n->bod.mass, n->bod.y * n->bod.mass);
}

double distance(double x1, double y1, double x2, double y2)
{
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

double dist1d(double a, double b)
{
    return std::sqrt(std::pow(a - b, 2));
}

void quadtree::calcBody(body& b, double time, double theta)
{
    if (b.mass < 0)
        return;
    double G = 0.0001;
    double rlim = 0.03;

    auto f = calcForce(parents[b.index], b, G, theta, rlim);

    double ax = f.x / b.mass;
    double ay = f.y / b.mass;
    b.x += (b.vx * time) + (0.5 * ax * std::pow(time, 2));
    b.y += (b.vy * time) + (0.5 * ay * std::pow(time, 2));
    b.vx += ax * time;
    b.vy += ay * time;

    if (b.x < 0 || b.x > 4 || b.y < 0 || b.x > 4)
        b.mass = -1;
}

std::shared_ptr<node> quadtree::findParent(std::shared_ptr<node> n, int index)
{
    if (!n || n->type == BODY)
        return nullptr;
    if (n->ne && n->ne->bod.index == index)
        return n;
    if (n->nw && n->nw->bod.index == index)
        return n;
    if (n->sw && n->sw->bod.index == index)
        return n;
    if (n->se && n->se->bod.index == index)
        return n;

    std::shared_ptr<node> toReturn = nullptr;
    toReturn = findParent(n->ne, index);
    if (toReturn)
        return toReturn;
    toReturn = findParent(n->nw, index);
    if (toReturn)
        return toReturn;
    toReturn = findParent(n->sw, index);
    if (toReturn)
        return toReturn;
    toReturn = findParent(n->se, index);
    if (toReturn)
        return toReturn;
    return nullptr;
}

force calcBodyForce(const body& b, const body& b2, double dist, double G)
{
    return force((G * b.mass * b2.mass * dist1d(b.x, b2.x)) / std::pow(dist, 3),
        (G * b.mass * b2.mass * dist1d(b.y, b2.y)) / std::pow(dist, 3));
}

force quadtree::calcForce(std::shared_ptr<node> n, const body& b, double G, double theta, double rlim)
{
    std::vector<std::shared_ptr<node>> children { n->ne, n->nw, n->se, n->sw };
    force total;
    for (auto& e : children) {
        if (!e)
            continue;
        if (e->bod.index == b.index)
            continue;
        auto d = distance(b.x, b.y, e->bod.x, e->bod.y);
        if (d < rlim)
            d = rlim;
        if (e->type == BODY) {
            total += calcBodyForce(b, e->bod, d, G);
            continue;
        }
        if (e->wind->getSideLength() / d < theta) {
            total += calcBodyForce(b, e->bod, d, G);
        } else {
            total += calcForce(e, b, G, theta, rlim);
        }
    }
    return total;
}

/*void quadtree::update(const std::vector<body>& vect)
{
    for (auto& e : vect) {
        removeBody(this->root, e.index);
        addNode(this->root, e);
    }
    calcCOM(root);
}

double quadtree::removeBody(std::shared_ptr<node> cur, int index)
{
    if (!cur)
        return 0;
    std::shared_ptr<node>* found = nullptr;
    if (cur->ne->bod.index == index)
        found = &cur->ne;
    if (cur->nw->bod.index == index)
        found = &cur->nw;
    if (cur->sw->bod.index == index)
        found = &cur->sw;
    if (cur->se->bod.index == index)
        found = &cur->se;
    if (found) {
        auto mass = (*found)->bod.mass;
        (*found) = nullptr;
        cur->bod.mass -= mass;
        return mass;
    }
    double toRemove = 0.0;
    toRemove += removeBody(cur->ne, index);
    toRemove += removeBody(cur->nw, index);
    toRemove += removeBody(cur->sw, index);
    toRemove += removeBody(cur->se, index);
    cur->bod.mass -= toRemove;
    return toRemove;
}*/