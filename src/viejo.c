/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h" 
#include "resource_dir.h"

void DrawAxes(float s);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - models loading");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 50.0f, 50.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 10.0f, 0.0f };     // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

    SearchAndSetResourceDir("resources");

    Model model = LoadModel("wallace.glb");                 // Load model
    Texture2D texture = LoadTexture("wabbit_alpha.png"); // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture

    if (model.meshes == NULL)
    {
        printf("No se cargo la cochinada. Ptamadre me cago en mi...");
        return 0;
    }

    Vector3 position = { 0.0f, 0.0f, 0.0f };                    // Set model position

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

    // NOTE: bounds are calculated from the original size of the model,
    // if model is scaled on drawing, bounds must be also scaled

    bool selected = false;          // Selected object flag

    DisableCursor();                // Limit cursor to relative movement inside the window

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Camera3D cam = { 0 };
    cam.fovy = 60;
    cam.projection = CAMERA_PERSPECTIVE;
    cam.position = (Vector3){ 0,1, 5 };
    cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Vector3 camDirection = { 0,0,-1 };
    //Vector ortogonal a la direccion de la camara
    Vector3 camDirectionRight = { 1,0,0 };

    //variables de control de camara
    float camSpeed = 1;


    float planetRot = 0;
    float moonRot = 0;

    //labels para el hud
    char* labelbuffer[64];



    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        camDirectionRight = Vector3Normalize(Vector3CrossProduct(camDirection, cam.up));
        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Load new models/textures on drag&drop
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1) // Only support one file dropped
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                    IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                    IsFileExtension(droppedFiles.paths[0], ".glb") ||
                    IsFileExtension(droppedFiles.paths[0], ".vox") ||
                    IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                    IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
                {
                    UnloadModel(model);                         // Unload previous model
                    model = LoadModel(droppedFiles.paths[0]);   // Load new model
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                    bounds = GetMeshBoundingBox(model.meshes[0]);

                    // TODO: Move camera position from target enough distance to visualize model properly
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
                {
                    // Unload current model texture and load new one
                    UnloadTexture(texture);
                    texture = LoadTexture(droppedFiles.paths[0]);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // Check collision between ray and box
            if (GetRayCollisionBox(GetScreenToWorldRay(GetMousePosition(), camera), bounds).hit) selected = !selected;
            else selected = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, position, 1.0f, WHITE);        // Draw 3d model with texture

        //DrawGrid(20,20);// Draw a grid
        DrawAxes(50);


        if (selected) DrawBoundingBox(bounds, GREEN);   // Draw selection box

        EndMode3D();

        DrawText("Drag & drop model to load mesh/texture.", 10, GetScreenHeight() - 20, 10, DARKGRAY);
        if (selected) DrawText("MODEL SELECTED", GetScreenWidth() - 110, 10, 10, GREEN);

        DrawText("(c) Castle 3D model by Alberto Cano", screenWidth - 200, screenHeight - 20, 10, GRAY);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);     // Unload texture
    UnloadModel(model);         // Unload model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void DrawAxes(float scale)
{
    float punta = scale / 10;
    float s = scale;
    rlBegin(RL_LINES);
    //eje X
    rlColor4ub(255, 0, 0, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(s, 0, 0);

    rlVertex3f(s, 0, 0);
    rlVertex3f(s - punta, 0, punta);

    rlVertex3f(s, 0, 0);
    rlVertex3f(s - punta, 0, -punta);
    //eje Y
    rlColor4ub(0, 255, 0, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(0, s, 0);

    rlVertex3f(0, s, 0);
    rlVertex3f(0, s - punta, punta);

    rlVertex3f(0, s, 0);
    rlVertex3f(0, s - punta, -punta);
    //eje Z
    rlColor4ub(0, 0, 255, 255);
    rlVertex3f(0, 0, 0);
    rlVertex3f(0, 0, s);

    rlVertex3f(0, 0, s);
    rlVertex3f(punta, 0, s - punta);

    rlVertex3f(0, 0, s);
    rlVertex3f(-punta, 0, s - punta);
    rlEnd();
}