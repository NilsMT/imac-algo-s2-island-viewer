#include "draw.hpp"

#include "app.hpp"

#include "generation.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

void draw3DScene(AppContext& context) {
    ClearBackground(RAYWHITE);
    
    BeginMode3D(context.camera);

    Matrix const terrainCentering { getTerrainCenteringMatrix(context) };
    Vector3 const terrainCenterOffset { terrainCentering.m12, terrainCentering.m13, terrainCentering.m14 };

    DrawModel(context.model, terrainCenterOffset, 1.0f, WHITE);
    drawCubes(context, terrainCentering);
    DrawGrid(20, 1.0f);

    EndMode3D();
}

void drawCubes(AppContext const& context, Matrix const& terrainCentering)
{
    if (context.objectPositions.empty()) {
        return;
    }

    float const cubeHalfHeight { 0.5f * context.cubeScale };

    srand(context.imageGenerationParameters.noiseSeed);

    for (glm::vec3 const& pos : context.objectPositions) {
        //classic params
        Matrix const objectTranslation {
            MatrixTranslate(
                pos.x * context.terrainSize.x,
                pos.z * context.terrainSize.y + cubeHalfHeight,
                pos.y * context.terrainSize.z
            )
        };
        Matrix const centeredTranslation {
            MatrixMultiply(objectTranslation, terrainCentering)
        };

        //randomization
        Vector3 scaleOff = {0,0,0};
        Vector3 rotOff = {0,0,0};
        auto randF = []() { return (float)rand() / (float)RAND_MAX; };

        if (context.pointsGenerationParameters.isScaleRandom) {
            scaleOff = {
                context.pointsGenerationParameters.scaleOffset[0] * randF(),
                context.pointsGenerationParameters.scaleOffset[1] * randF(),
                context.pointsGenerationParameters.scaleOffset[2] * randF()
            };
        };

        if (context.pointsGenerationParameters.isRotationRandom) {
            rotOff = {
                glm::radians(context.pointsGenerationParameters.rotationOffset[0]) * randF(),
                glm::radians(context.pointsGenerationParameters.rotationOffset[1]) * randF(),
                glm::radians(context.pointsGenerationParameters.rotationOffset[2]) * randF()
            };
        };

        //compute matrix
        Matrix localScale   { MatrixScale(context.cubeScale + scaleOff.x,
                                        context.cubeScale + scaleOff.y,
                                        context.cubeScale + scaleOff.z) };
        Matrix localRotate  { MatrixRotateXYZ(rotOff) };

        //scale then rotate
        Matrix localTransform { MatrixMultiply(localScale, localRotate) };

        // Then apply world-space translation last
        Matrix transform { MatrixMultiply(localTransform, centeredTranslation) };

        DrawMesh(context.cube, context.cubeMaterial, transform);
    }
}

void drawImGui(AppContext& context) {
    if (ImGui::CollapsingHeader("Height map", ImGuiTreeNodeFlags_DefaultOpen)) {
        //checkbox for random seed
        ImGui::Checkbox("Random Seed",&context.imageGenerationParameters.isSeedRandom);

        //manual seed
        ImGui::InputInt("Seed",&context.imageGenerationParameters.noiseSeed);


        
        ImGui::Separator();


        
        //image resolution
        ImGui::SliderInt("Resolution",&context.imageGenerationParameters.resolution,16,1024);

        //noise scale
        ImGui::SliderFloat("Noise Scale",&context.imageGenerationParameters.noiseScale, 0.01f, 10.0f);

        //color map
        ImGui::Combo(
            "Color Map", 
            &context.imageGenerationParameters.selectedColorMap, 
            context.imageGenerationData.colorMaps, 
            IM_ARRAYSIZE(context.imageGenerationData.colorMaps)
        );
        


        ImGui::Separator();


        
        //reload heightmap color
        if (ImGui::Button("Reload Color Map")) {
            generateHeightmap(context);
        }

        //regen heightmap
        if (ImGui::Button("Regenerate Heightmap")) {
            setupSeed(context);
            generateHeightmap(context);
        }

        //regen mesh
        if (ImGui::Button("Regenerate Mesh")) {
            regenerateMeshFromImage(context);
        }
    }



    ImGui::Separator();


    
    if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
        //cube size
        ImGui::SliderFloat("Cube Scale", &context.cubeScale, 0.01f, 1.0f);



        ImGui::Separator();


        
        //checkbox for random object rotation
        ImGui::Checkbox("Random Object Rotations",&context.pointsGenerationParameters.isRotationRandom);

        //params for random object rotation
        //https://github.com/ocornut/imgui/issues/779
        if (context.pointsGenerationParameters.isRotationRandom) {
            ImGui::SliderFloat3("Rotation Offset", (float*)&context.pointsGenerationParameters.rotationOffset, 0.0f, 360.0f);
        }

        //checkbox for random object scale
        ImGui::Checkbox("Random Object Scale",&context.pointsGenerationParameters.isScaleRandom);

        //params for random object scale
        //https://github.com/ocornut/imgui/issues/779
        if (context.pointsGenerationParameters.isRotationRandom) {
            ImGui::SliderFloat3("Scale Offset", (float*)&context.pointsGenerationParameters.scaleOffset, 0.0f, 10.0f);
        }


        
        ImGui::Separator();


        
        //replace cubes
        if(ImGui::Button("Regenerate Objects")) {
            generateObjectsPositions(context);
        }
    }



    ImGui::Separator();


    
    //regen all
    if (ImGui::Button("Regenerate All")) {
        generateHeightmap(context);
        regenerateMeshFromImage(context);
        generateObjectsPositions(context);
    }
}

void drawRaylibUI(AppContext& context) {
    int screenWidth { GetScreenWidth() };
    
    float wanted_size { 400.f };
    float scale_factor { wanted_size / std::max(context.texture.width, context.texture.height) };
    float const preview_x { screenWidth - wanted_size - 20.f };
    float const preview_y { 20.f };
    float const preview_w { context.texture.width * scale_factor };
    float const preview_h { context.texture.height * scale_factor };
    // DrawTexture(context.texture, screenWidth - context.texture.width - 20, 20, WHITE);
    DrawTextureEx(context.texture, { preview_x, preview_y }, 0.0f, scale_factor, WHITE);
    DrawRectangleLines(screenWidth - wanted_size - 20, 20, wanted_size, wanted_size, GREEN);

    //draw positions on top of the heightmap
    for (auto const& pos : context.objectPositions)
    {
        // Remap normalized coordinates [0..1] to the preview image in screen space.
        float const px { preview_x + Clamp(pos.x, 0.0f, 1.0f) * preview_w };
        float const py { preview_y + Clamp(pos.y, 0.0f, 1.0f) * preview_h };

        DrawCircleV({ px, py }, 2.0f, RED);
    }

    DrawFPS(10, 10);
}

