#pragma once

typedef struct Vec2{
    float x;
    float y;
} Vec2;

typedef struct Vec3{
    float x;
    float y;
    float z;
} Vec3;

typedef struct Vec4{
    float x;
    float y;
    float z;
    float w;
} Vec4;


Vec2 vec2(float x, float y){
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}
Vec2 vec2s(float s){
    return vec2(s, s);
}
Vec2 vec2_add(Vec2 a, Vec2 b){
    return vec2(a.x + b.x, a.y + b.y);
}
Vec2 vec2_sub(Vec2 a, Vec2 b){
    return vec2(a.x - b.x, a.y - b.y);
}
Vec2 vec2_mul(Vec2 a, Vec2 b){
    return vec2(a.x * b.x, a.y * b.y);
}

Vec2 vec2_div(Vec2 a, Vec2 b){
    return vec2(a.x / b.x, a.y / b.y);
}
Vec2 vec2_scale(Vec2 a, float s){
    return vec2(a.x * s, a.y * s);
}
