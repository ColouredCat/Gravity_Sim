
/*
GRAVITY SIM
A simulation of the effects of gravity in a vacum
Written by Robert Jordan
*/

#include "raylib.h"
#include <stdio.h>
#include "raymath.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

//include emscripten for web builds
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define FPS 60
#define CAM_ROTATE_SPEED 0.7f
#define MAX_OBJECTS 50

#define G 6.6743*pow(10, -11)
#define VEL_CHANGE 0.000001

#define BACKGROUND BLACK
#define FOREGROUND WHITE
#define TEXT_SCALE 20
#define TEXT_COLOUR (Color){15, 189, 61, 255}

#define randint(a,b) (rand()%(b-a)) - b

//global variables
int WIDTH, HEIGHT;
float vel_scale = 0.00001;
Camera camera = { 0 };

// Single Object Functions

struct object {
    Vector3 pos, vel;
    Color col;
    float radius, mass, density, volume;
};

void calculate_radius(struct object* b){
    //calculate the correct radius to draw of an object
    //volume = mass/density
    b->volume = b->mass/b->density;
    //volume = (4/3)*PI*r^3 so r = cbrt(volume/(4/3)*PI)
    b->radius = cbrt(b->volume/((4/3)*PI));
}

void calculate_gravity(struct object* b1, struct object* b2){
    //first, calculate the distance between the two objects
    double xdif = b1->pos.x - b2->pos.x;
    double ydif = b1->pos.y - b2->pos.y;
    double zdif = b1->pos.z - b2->pos.z;
    double dist = sqrt(pow(xdif, 2) + pow(ydif, 2) + pow(zdif, 2));
    //printf("Distance : %f\n", (float)dist);

    //force(x/y/z) = force_m * cos(angle between (x/y/z) and force)
    double force_m = (G * b1->mass * b2->mass) / pow(dist, 2);
    //printf("%d\n", force_m);
    b1->vel.x -= (force_m * (xdif/force_m))*vel_scale;
    b1->vel.y -= (force_m * (ydif/force_m))*vel_scale;
    b1->vel.z -= (force_m * (zdif/force_m))*vel_scale;
    //printf("Velocity : %f, %f, %f\n", y1->vel.x, y1->vel.y, y1->vel.z);
}

void update_object(struct object* b){
    //update position
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;
    b->pos.z += b->vel.z;
    //draw the object
    DrawSphere(b->pos, b->radius, b->col);
    DrawSphereWires(b->pos, b->radius, 10, 8, FOREGROUND);
}

void check_collision(struct object* b1, struct object* b2){
    if (CheckCollisionSpheres(b1->pos, b1->radius, b2->pos, b2->radius)){
        //calculate the average density weighted by mass
        b1->density = (b1->density*b1->mass + b2->density*b2->mass) / (b1->mass + b2->mass);

        //conserve mass
        b1->mass += b2->mass;
        b2->mass = 0;
        calculate_radius(b1);
        calculate_radius(b2);

        //conserve momentum
        b1->vel.x += b2->vel.x;
        b1->vel.y += b2->vel.y;
        b1->vel.z += b2->vel.z;
    }
}


// Multiple Object Functions

struct object x[MAX_OBJECTS];
void update_objects(){
    for (int a = 0; a < MAX_OBJECTS; a++){
        if (x[a].mass != 0) { update_object(&x[a]); }
        for (int b = 0; b < MAX_OBJECTS; b++){
            if (a != b && x[a].mass != 0 && x[b].mass != 0){
                calculate_gravity(&x[a], &x[b]);
                check_collision(&x[a], &x[b]);
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

    //speed up or slow down the simulation
    if (IsKeyDown(KEY_PERIOD) && vel_scale < 1){ vel_scale += VEL_CHANGE; }
    else if (IsKeyDown(KEY_COMMA) && vel_scale > 0){ vel_scale -= VEL_CHANGE; }
}

// scenes 
// have a short description for each scene
char* scene_text;

void scene_1(){
    scene_text = "Scene 1 : Two bodies orbiting a dense central mass";
    for (int i = 0; i < MAX_OBJECTS; i++) {x[i].mass = 0;}
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

void scene_2(){
    scene_text = "Scene 2 : Two bodies coliding into each other";
    for (int i = 0; i < MAX_OBJECTS; i++) {x[i].mass = 0;}
    x[0].pos = (Vector3){0.0f, 0.0f, 0.0f};
    x[0].vel = (Vector3){-0.0f, 0.0f, 0.0f};
    x[0].mass = 1000;
    x[0].density = 100;
    calculate_radius(&x[0]);

    x[1].pos = (Vector3){17.0f, 10.0f, 13.0f};
    x[1].vel = (Vector3){-0.0f, -0.0f, -0.0f};
    x[1].mass = 1000;
    x[1].density = 100;
    calculate_radius(&x[1]);
}

void scene_3(){
    const int max_dist = 75;
    const int min_dist = -75;
    const int num_objects = 50;
    const Color colours[5] = {RED, BLUE, GREEN, PURPLE, ORANGE};
    scene_text = "Scene 3 : Many objects clumping together";

    for (int i = 0; i < num_objects; i++) {
        x[i].pos = (Vector3){randint(min_dist, max_dist), randint(min_dist, max_dist), randint(min_dist, max_dist)};
        x[i].vel = (Vector3){-0.0f, 0.0f, 0.0f};
        x[i].mass = randint(10, 100);
        x[i].density = 10;
        x[i].col = colours[rand()%5];
        calculate_radius(&x[i]);
    }
}

void scene_4(){
    const int max_dist = 150;
    const int min_dist = -150;
    const int num_objects = 30;
    const Color colours[5] = {RED, BLUE, GREEN, PURPLE, ORANGE};
    scene_text = "Scene 4 : Ring of particles orbiting a central mass";

    for (int i = 0; i < num_objects; i++) {
        x[i].pos = (Vector3){randint(min_dist, max_dist), randint(min_dist, max_dist), randint(min_dist, max_dist)};
        x[i].vel = (Vector3){randint(-1, 1)/10, randint(-1, 1)/10, randint(-1, 1)/10};
        x[i].mass = randint(1, 10);
        x[i].density = 1;
        x[i].col = colours[rand()%5];
        calculate_radius(&x[i]);
    }

    x[0].pos = (Vector3){0.0f, 0.0f, 0.0f};
    x[0].vel = (Vector3){-0.0f, 0.0f, 0.0f};
    x[0].mass = 1000;
    x[0].density = 100;
    calculate_radius(&x[0]);
}

void check_scene_change(){
    if (IsKeyPressed(KEY_ONE)){
        scene_1();
    } else if (IsKeyPressed(KEY_TWO)){
        scene_2();
    } else if (IsKeyPressed(KEY_THREE)){
        scene_3();
    } else if (IsKeyPressed(KEY_FOUR)){
        scene_4();
    }
}

void draw_frame(){
    char vel_text[30];
    
    BeginDrawing();

    ClearBackground(BACKGROUND);
    rotate_camera(&camera);
    check_scene_change();

    BeginMode3D(camera);
    update_objects();
    DrawGrid(50, 2.0f);
    EndMode3D();

    DrawFPS(50,50);
    DrawText(scene_text, WIDTH/2, 50, TEXT_SCALE, TEXT_COLOUR);

    sprintf(vel_text, "Velocity scale : %f", vel_scale);
    DrawText(vel_text, 50, 100, TEXT_SCALE, TEXT_COLOUR);
    DrawText("Controls :\n > / < to increase/decrease velocity\n WASD to move camera\n 12345 to select scene", 50, 150, TEXT_SCALE, TEXT_COLOUR);

    EndDrawing();
}

int main(){

    #if defined(PLATFORM_WEB)
        WIDTH = 1200;
        HEIGHT = 700;
    #else
        //otherwise, use maximum screen size posible
        int display = GetCurrentMonitor();
        WIDTH = GetMonitorWidth(display);
        HEIGHT = GetMonitorHeight(display);
    #endif

    //setup window
    InitWindow(WIDTH, HEIGHT, "Gravity");
    ToggleFullscreen();

    time_t t = clock();
    srand(t);

    //setup camera so it is looking down on the scene
    camera.position = (Vector3){ 30.0f, 30.0f, 30.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    scene_1();

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(draw_frame, 0, 1);
    #else
        SetTargetFPS(FPS);
        //mainloop
        while (!WindowShouldClose()) {
            draw_frame();
        }
    #endif
    
    CloseWindow();
}