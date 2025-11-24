
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define WIDTH 1200
#define HEIGHT 700
#define FPS 60
#define CAM_ROTATE_SPEED 0.5f
#define NUM_OBJECTS 3
#define G 6.6743*pow(10, -12)
#define BACKGROUND BLACK
#define FOREGROUND WHITE
#define SCENE_NUM 2
#define VEL_SCALE 0.0001

// Single Object Functions

struct object {
    Vector3 pos, vel;
    Color col;
    float radius, mass, density, volume;
};

void calculate_radius(struct object* y){
    //calculate the correct radius to draw of an object
    //volume = mass/density
    y->volume = y->mass/y->density;
    //volume = (4/3)*PI*r^3 so r = cbrt(volume/(4/3)*PI)
    y->radius = cbrt(y->volume/((4/3)*PI));
}

void calculate_gravity(struct object* y1, struct object* y2){
    //first, calculate the distance between the two objects
    double xdif = y1->pos.x - y2->pos.x;
    double ydif = y1->pos.y - y2->pos.y;
    double zdif = y1->pos.z - y2->pos.z;
    double dist = sqrt(pow(xdif, 2) + pow(ydif, 2) + pow(zdif, 2));
    //printf("Distance : %f\n", (float)dist);

    /*1. applying newtons law of univeral gravitation  - doesn't realy work
    y1->vel.x -= (G * y1->mass * y2->mass * xdif) / pow(dist, 3);
    y1->vel.y -= (G * y1->mass * y2->mass * ydif) / pow(dist, 3);
    y1->vel.z -= (G * y1->mass * y2->mass * zdif) / pow(dist, 3);*/
    //2. force(x/y/z) = force_m * cos(angle between (x/y/z) and force)
    double force_m = (G * y1->mass * y2->mass) / pow(dist, 2);
    //printf("%d\n", force_m);
    y1->vel.x -= (force_m * (xdif/force_m))*VEL_SCALE;
    y1->vel.y -= (force_m * (ydif/force_m))*VEL_SCALE;
    y1->vel.z -= (force_m * (zdif/force_m))*VEL_SCALE;
    //printf("Velocity : %f, %f, %f\n", y1->vel.x, y1->vel.y, y1->vel.z);
}

void update_object(struct object* y){
    //update position
    y->pos.x += y->vel.x;
    y->pos.y += y->vel.y;
    y->pos.z += y->vel.z;
    //draw the object
    DrawSphere(y->pos, y->radius, y->col);
    DrawSphereWires(y->pos, y->radius, 10, 8, FOREGROUND);
}

// Multiple Object Functions

struct object x[NUM_OBJECTS];
void update_objects(){
    for (int a = 0; a < NUM_OBJECTS; a++){
        update_object(&x[a]);
        for (int b = 0; b < NUM_OBJECTS; b++){
            if (a != b){
                calculate_gravity(&x[a], &x[b]);
            }
        }
    }
}

// Camera Functions

void rotate_camera(Camera* camera){
    //rotate the camera on certain key presses
    Vector2 mouse = GetMouseWheelMoveV();
    if (IsKeyDown(KEY_RIGHT)){ camera->position.x += CAM_ROTATE_SPEED; }
    else if (IsKeyDown(KEY_LEFT)){ camera->position.x -= CAM_ROTATE_SPEED; }
    if (IsKeyDown(KEY_UP)){ camera->position.y += CAM_ROTATE_SPEED; }
    else if (IsKeyDown(KEY_DOWN)){ camera->position.y -= CAM_ROTATE_SPEED; }
    if (mouse.x > 0){ camera->position.z += CAM_ROTATE_SPEED*3; }
    else if (mouse.y < 0){ camera->position.z -= CAM_ROTATE_SPEED*3; }
}

// Scenarios 

void scenario_1(){
    for (int i = 0; i < NUM_OBJECTS; i++) {x[i].radius = 0;}
    x[0].pos = (Vector3){0.0f, 0.0f, 0.0f};
    x[0].vel = (Vector3){0.0f, 0.0f, 0.0f};
    x[0].mass = pow(10, 10);
    x[0].density = pow(8, 10);
    x[0].col = RED;
    calculate_radius(&x[0]);

    x[1].pos = (Vector3){17.0f, 10.0f, 13.0f};
    x[1].vel = (Vector3){-0.1f, -0.3f, -0.2f};
    x[1].mass = 22000000000;
    x[1].density = 1000000000;
    x[1].col = BLUE;
    calculate_radius(&x[1]);

    x[2].pos = (Vector3){7.0f, 0.0f, 3.0f};
    x[2].vel = (Vector3){0.1f, 0.3f, 0.2f};
    x[2].mass = 3200000;
    x[2].density = 200000;
    x[2].col = BLUE;
    calculate_radius(&x[2]);
}

/*
void scenario_2(){
    for (int i = 0; i < NUM_OBJECTS; i++) {x[i].radius = 0;}
    x[0].pos = (Vector3){0.0f, 0.0f, 0.0f};
    x[0].vel = (Vector3){-0.0f, 0.0f, 0.0f};
    x[0].mass = pow(10, 10);
    x[0].density = pow(7, 10);
    calculate_radius(&x[0]);

    x[1].pos = (Vector3){17.0f, 10.0f, 13.0f};
    x[1].vel = (Vector3){-0.0f, -0.3f, -0.2f};
    x[1].mass = 22000;
    x[1].density = 1000;
    calculate_radius(&x[1]);
    
    x[2].pos = (Vector3){7.0f, 2.0f, 3.0f};
    x[2].vel = (Vector3){0.1f, 0.2f, 0.05f};
    x[2].mass = 0.22;
    x[2].density = 0.025;
    calculate_radius(&x[2]);
}

//store the scenario descriptions
const char* s_descriptions[SCENE_NUM] = {"Scenario 1 : Two bodies orbiting a dense cental mass", 
    "Scenario 2 : One body orbiting a dense cental mass",};
int s_current = 0;

void check_scene_change(){
    if (IsKeyPressed(KEY_KP_1)){
        scenario_1();
        s_current = 1;
    } else if (IsKeyPressed(KEY_KP_2)){
        scenario_2();
        s_current = 2;
    }
}*/

int main(){
    //setup window
    InitWindow(WIDTH, HEIGHT, "Gravity");
    SetTargetFPS(FPS);
    int display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    ToggleFullscreen();

    //setup camera so it is looking down on the scene
    Camera camera = { 0 };
    camera.position = (Vector3){ 30.0f, 30.0f, 30.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    scenario_1();
    
    //mainloop
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BACKGROUND);
        rotate_camera(&camera);
        //check_scene_change();

        BeginMode3D(camera);
        update_objects();
        DrawGrid(30, 1.0f);
        EndMode3D();

        DrawFPS(50,50);
        //DrawText(s_descriptions[s_current], WIDTH/2, 50, 20, DARKGREEN);

        EndDrawing();
    }
    
    CloseWindow();
}
