#include "Test.h"

using namespace godot;

void Test::_register_methods() {
    register_method("_process", &Test::_process);
    register_property<Test, int>("a", &Test::a, 0);
}

Test::Test() {
    Godot::print("construct");
}

Test::~Test() {
    Godot::print("destruct");
    // add your cleanup here
}

void Test::_init() {
    time = 0;
    a = 0;
    Godot::print("init");
    // initialize any variables here
}

void Test::_process(float delta) {
    time += delta;
    if (time > 1.0) {
        Godot::print("time:" + String::num(time));
        time = 0;
    }
}
