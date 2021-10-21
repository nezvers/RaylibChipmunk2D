/*
 * Copyright (c) 2019 Chris Camacho (codifies -  http://bedroomcoders.co.uk/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stddef.h>

#include "raylib.h"
#include <chipmunk/chipmunk.h>


#define screenWidth 1280
#define screenHeight 720

enum shapeType {
    s_circle = 0,
    s_box = 1
};

typedef struct phys {
    enum shapeType type;
    Vector2 size;
    cpBody* body;
    cpShape* shape;
    bool sharedBody;
} phys;


#define MaxObj 40
phys Obj[MaxObj];
cpSpace* space;
Sound bang;

// o is first Obj index, pivots use 2 Obj's
cpConstraint* makePivot(int o, cpVect pos)
{
    Obj[o].type = s_box;
    Obj[o].size = (Vector2){ 96,8 };
    Obj[o].body = cpSpaceAddBody(space, cpBodyNew(0, 0));
    Obj[o].sharedBody = false;
    Obj[o].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[o].body, Obj[o].size.x, Obj[o].size.y, 0.0));
    cpShapeSetFriction(Obj[o].shape, 0.6);
    cpShapeSetMass(Obj[o].shape, 8);

    Obj[o + 1].type = s_box;
    Obj[o + 1].size = (Vector2){ 8,96 };
    Obj[o + 1].body = Obj[o].body;
    Obj[o + 1].sharedBody = true;
    Obj[o + 1].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[o].body, Obj[o + 1].size.x, Obj[o + 1].size.y, 0.0));
    cpShapeSetFriction(Obj[o + 1].shape, 0.6);
    cpShapeSetMass(Obj[o + 1].shape, 8);

    cpBodySetPosition(Obj[o].body, pos);
    return cpSpaceAddConstraint(space, cpPivotJointNew(Obj[o].body, cpSpaceGetStaticBody(space), pos));
}

void postSolve(cpArbiter* arb, cpSpace* space, cpDataPointer data)
{
    (void)space;
    (void)data;
    if (cpArbiterIsFirstContact(arb)) {
        //TraceLog(LOG_INFO,"total KE %f", cpArbiterTotalKE(arb));
        float f = cpArbiterTotalKE(arb);
        if (f > 20000) {
            f = 20000;
        }
        if (f > 10000) {
            /* play sound of impact */
            //SetSoundVolume(bang, (f - 6000) / 20000);
            //PlaySoundMulti(bang);
        }
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib - chipmunk test");

    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth / 2.0, screenHeight / 2.0 };
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    //InitAudioDevice();
    //bang = LoadSound("data/bang.wav");

    // frame counter
    int frame = 0;

    // for mouse dragging
    Vector2 lastMouse = { 0.0f, 0.0f };
    Vector2 mouse = { 0.0f, 0.0f };
    bool dragging = false;

    cpVect gravity = cpv(0, 98);
    space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);

    cpCollisionHandler* handler = cpSpaceAddCollisionHandler(space, 0, 0);
    handler->postSolveFunc = (cpCollisionPostSolveFunc)postSolve;

    cpShape* slope = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(120, 400), cpv(1160, 800), 0);
    cpShapeSetFriction(slope, .8);
    cpSpaceAddShape(space, slope);



    for (int n = 6; n < MaxObj; n++) {
        cpFloat mass;
        if (n % 2) {
            Obj[n].type = s_box;
            Obj[n].size.x = GetRandomValue(24, 48);
            Obj[n].size.y = GetRandomValue(24, 48);
            mass = (Obj[n].size.x * Obj[n].size.y) * 0.001;
            Obj[n].body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, Obj[n].size.x, Obj[n].size.y)));
            Obj[n].sharedBody = false;
            Obj[n].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[n].body, Obj[n].size.x, Obj[n].size.y, 0.0));
            cpShapeSetFriction(Obj[n].shape, 0.4);
        }
        else {
            Obj[n].type = s_circle;
            Obj[n].size.x = GetRandomValue(12, 24);
            mass = (PI * (Obj[n].size.x * Obj[n].size.x)) * 0.001;
            Obj[n].body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0, Obj[n].size.x, cpvzero)));
            Obj[n].sharedBody = false;
            Obj[n].shape = cpSpaceAddShape(space, cpCircleShapeNew(Obj[n].body, Obj[n].size.x, cpvzero));
            cpShapeSetFriction(Obj[n].shape, 0.7);
        }
        cpBodySetPosition(Obj[n].body, cpv(GetRandomValue(120, 1160), GetRandomValue(400, 500)));

    }

    cpConstraint* joints[3];

    joints[0] = makePivot(0, (cpVect) { 300, 300 });
    joints[1] = makePivot(2, (cpVect) { 450, 300 });
    joints[2] = makePivot(4, (cpVect) { 600, 300 });



    SetTargetFPS(60);               // Set  to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        frame++;

        cpSpaceStep(space, 1.0 / 60.0);

        // fake joint friction
        const float drag = 0.999;
        cpBodySetAngularVelocity(Obj[0].body, cpBodyGetAngularVelocity(Obj[0].body) * drag);
        cpBodySetAngularVelocity(Obj[2].body, cpBodyGetAngularVelocity(Obj[2].body) * drag);
        cpBodySetAngularVelocity(Obj[4].body, cpBodyGetAngularVelocity(Obj[4].body) * drag);



        lastMouse = mouse;
        mouse = GetMousePosition();

        float mz = ((float)GetMouseWheelMove()) / 10.0;
        if (mz != 0.0) {
            camera.zoom += mz;
            if (camera.zoom < 0.1) {
                camera.zoom = 0.1;
            }
        }


        if (dragging) {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragging = false;
            float dx = ((float)(mouse.x - lastMouse.x));
            float dy = ((float)(mouse.y - lastMouse.y));
            camera.offset.x += dx;
            camera.offset.y += dy;
            camera.target = camera.offset;
        }
        else {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                dragging = true;
            }
        }



        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camera);
        // TODO should get shape offset...
        for (int n = 0; n < MaxObj; n++) {
            cpVect pos = cpBodyGetPosition(Obj[n].body);
            float a = cpBodyGetAngle(Obj[n].body);
            if (Obj[n].type == s_circle) {
                cpFloat r = cpCircleShapeGetRadius(Obj[n].shape);
                DrawCircle(pos.x, pos.y, r, WHITE);
                DrawLine(pos.x, pos.y,
                    pos.x + cos(a) * Obj[n].size.x, pos.y + sin(a) * Obj[n].size.x, BLUE);
            }
            else {
                DrawRectanglePro((Rectangle) { pos.x, pos.y, Obj[n].size.x, Obj[n].size.y },
                    (Vector2) {
                    Obj[n].size.x / 2, Obj[n].size.y / 2
                },
                    cpBodyGetAngle(Obj[n].body)* RAD2DEG, RED);
                Vector2 p[4] = { 0 };
                Vector2 sz = (Vector2){ Obj[n].size.x / 2.0, Obj[n].size.y / 2.0 };
                p[0] = (Vector2){ -sz.x, -sz.y };
                p[1] = (Vector2){ sz.x, -sz.y };
                p[2] = (Vector2){ sz.x,  sz.y };
                p[3] = (Vector2){ -sz.x,  sz.y };

                for (int i = 0; i < 4; i++) {
                    float x = p[i].x;
                    float y = p[i].y;
                    p[i].x = (cos(a) * x) - (sin(a) * y) + pos.x;
                    p[i].y = (cos(a) * y) + (sin(a) * x) + pos.y;
                }
                DrawLine(p[0].x, p[0].y, p[1].x, p[1].y, BLUE);
                DrawLine(p[1].x, p[1].y, p[2].x, p[2].y, BLUE);
                DrawLine(p[2].x, p[2].y, p[3].x, p[3].y, BLUE);
                DrawLine(p[3].x, p[3].y, p[0].x, p[0].y, BLUE);

            }


            if (pos.y > (screenHeight + 200)) {
                cpBodySetPosition(Obj[n].body, cpv(GetRandomValue(220, 520), GetRandomValue(100, 250)));
                cpBodySetVelocity(Obj[n].body, cpvzero);
            }
        }


        DrawLine(120, 400, 1160, 800, WHITE);
        EndMode2D();

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int n = 0; n < MaxObj; n++) {
        if (!Obj[n].sharedBody) {
            cpBodyFree(Obj[n].body);
        }
        cpShapeFree(Obj[n].shape);
    }
    cpConstraintFree(joints[0]);
    cpConstraintFree(joints[1]);
    cpConstraintFree(joints[2]);
    cpShapeFree(slope);
    cpSpaceFree(space);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


