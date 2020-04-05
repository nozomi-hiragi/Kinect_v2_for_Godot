#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <Godot.hpp>
#include <Sprite.hpp>

namespace godot {

class Test : public Node {
    GODOT_CLASS(Test, Node)

private:
    float time;
    int a;

public:
    static void _register_methods();

    Test();
    ~Test();

    void _init(); // our initializer called by Godot

    void _process(float delta);
};

}

#endif