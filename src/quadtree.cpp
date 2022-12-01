#include "quadtree.hpp"

#include <iostream>

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
    if (b->x < x_cen && b->y < y_cen)
        next = &cur->sw;
    else if (b->x < x_cen && b->y >= y_cen)
        next = &cur->nw;
    else if (b->x >= x_cen && b->y < y_cen)
        next = &cur->se;
    else if (b->x >= x_cen && b->y >= y_cen)
        next = &cur->ne;
    else
    {
        std::cerr << "should fit in a quadrant" << std::endl;
        exit(1);
    }

    if (*next)
    {
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
    }
}

// double x_cen, y_cen;
// std::tie(x_cen, y_cen) = wind->getCenter();
// root->nw = new node(new window(wind->x_min, x_cen, y_cen, wind->y_max));
// root->ne = new node(new window(x_cen, wind->x_max, y_cen, wind->y_max));
// root->sw = new node(new window(wind->x_min, x_cen, wind->y_min, y_cen));
// root->se = new node(new window(x_cen, wind->x_max, wind->y_min, y_cen));
