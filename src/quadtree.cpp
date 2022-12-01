#include "quadtree.hpp"

#include <iostream>

void body::readIn(std::ifstream &in)
{
    in >> this->bodyIndex;
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

node::node(body * b)
{
    this->type = BODY;
    this->self_body = b;
    this->self_window = nullptr;
    node();
}

node::node(window *w)
{
    this->type = BODY;
    this->self_body = nullptr;
    this->self_window = w;
    node();
}

quadtree::quadtree(std::vector<body*> init, window *wind)
{
    root = new node(wind);
    root->self_body = new body();
    for(auto& e : init) 
    {
        addNode(root, e);
    }
}

void quadtree::addNode(node *cur, body *b)
{
    if (!b)
    {
        std::cerr << "unexpected null body" << std::endl;
        exit(1);
    }

    if(!cur) return;

    cur->self_body->mass += b->mass;
    double x_cen, y_cen;
    std::tie(x_cen, y_cen) = cur->self_window->getCenter();

    node **next = nullptr;
    window * newWind = nullptr;
    auto curWindow = cur->self_window;

    auto avgX = [](window *w)
    { return (w->x_max + w->x_min) / 2.0; };
    
    auto avgY = [](window *w)
    { return (w->y_max + w->y_min) / 2.0; };

    if (b->x < x_cen && b->y < y_cen)
    {
        next = &cur->sw;
        newWind = new window(curWindow->x_min, avgX(curWindow), curWindow->y_min, avgY(curWindow));
    }
    else if (b->x < x_cen && b->y >= y_cen)
    {
        next = &cur->nw;
        newWind = new window(curWindow->x_min, avgX(curWindow), avgY(curWindow), curWindow->y_max);
    }
    else if (b->x >= x_cen && b->y < y_cen)
    {
        next = &cur->se;
        newWind = new window(avgX(curWindow), curWindow->x_max, curWindow->y_min, avgY(curWindow));
    }
    else if (b->x >= x_cen && b->y >= y_cen)
    {
        next = &cur->ne;
        newWind = new window(avgX(curWindow), curWindow->x_max, avgY(curWindow), curWindow->y_max);
    }
    else
    {
        std::cerr << "should fit in a quadrant" << std::endl;
        exit(1);
    }

    if (*next)
    {
        delete newWind;
        if ((*next)->type == BODY)
        {
            auto tmpBody = (*next)->self_body;
            (*next)->type = WINDOW;
            (*next)->self_body = new body();
            addNode(*next, tmpBody);
        }
        addNode(*next, b);
    }
    else
    {
        *next = new node(b);
        (*next)->self_window = newWind;
    }
}
