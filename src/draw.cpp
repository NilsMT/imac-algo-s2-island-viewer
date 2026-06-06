#include "draw.hpp"
#include "app.hpp"
#include "generation.hpp"
#include "utils/randFloat.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

void draw3DScene(AppContext& context) {
    ClearBackground(RAYWHITE);
    
    BeginMode3D(context.camera);

    Matrix const terrainCentering { getTerrainCenteringMatrix(context) };
    Vector3 const terrainCenterOffset { terrainCentering.m12, terrainCentering.m13, terrainCentering.m14 };

    DrawModel(context.model, terrainCenterOffset, 1.0f, WHITE);
    drawCones(context, terrainCentering);
    DrawGrid(20, 1.0f);

    EndMode3D();
}

void drawCones(AppContext const& context, Matrix const& terrainCentering)
{
    if (context.objectPositions.empty()) {
        return;
    }

    float const coneHalfHeight { 0.5f * context.coneScale };

    for (long long unsigned int i = 0; i < context.objectPositions.size(); i++) { 
        //^ the type of i is supposed to be in but the compiler was cring over "I prefer long long unsigned int" ☝️🤓
        glm::vec3 const& pos = context.objectPositions[i];
        ObjectRandomizationData objRand = context.objectsRandData[i];

        //classic params
        Matrix const objectTranslation {
            MatrixTranslate(
                pos.x * context.terrainSize.x,
                pos.z * context.terrainSize.y + coneHalfHeight,
                pos.y * context.terrainSize.z
            )
        };
        Matrix const centeredTranslation {
            MatrixMultiply(objectTranslation, terrainCentering)
        };

        //randomization
        Vector3 scaleOff = {0,0,0};
        Vector3 rotOff = {0,0,0};

        if (context.pointsGenerationParameters.isScaleRandom) {
            scaleOff = {
                context.pointsGenerationParameters.scaleOffset[0] * objRand.scaleOffset.x,
                context.pointsGenerationParameters.scaleOffset[1] * objRand.scaleOffset.y,
                context.pointsGenerationParameters.scaleOffset[2] * objRand.scaleOffset.z
            };
        };

        if (context.pointsGenerationParameters.isRotationRandom) {
            rotOff = {
                glm::radians(context.pointsGenerationParameters.rotationOffset[0]) * objRand.rotOffset.x,
                glm::radians(context.pointsGenerationParameters.rotationOffset[1]) * objRand.rotOffset.y,
                glm::radians(context.pointsGenerationParameters.rotationOffset[2]) * objRand.rotOffset.z
            };
        };

        //compute matrix
        Matrix localScale = MatrixScale(
            context.coneScale + scaleOff.x,
            context.coneScale + scaleOff.y,
            context.coneScale + scaleOff.z
        );
        Matrix localRotate = MatrixRotateXYZ(rotOff);

        //scale then rotate
        Matrix localTransform = MatrixMultiply(localScale, localRotate);

        // Then apply world-space translation last
        Matrix transform = MatrixMultiply(localTransform, centeredTranslation);

        DrawMesh(context.cone, context.coneMaterial, transform);
    }
}

// Utility: one call replaces ImGuiCustomSpace
static void Spacing(int n = 1) {
    for (int i = 0; i < n; i++) ImGui::Spacing();
}

void drawImGui(AppContext& context) {
    // Tab bar - replaces the three top-level CollapsingHeaders
    if (ImGui::BeginMainMenuBar()) {

        //----------------------------------------------
        // HEIGHT MAP  (blue accent)
        //----------------------------------------------
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.15f, 0.30f, 0.55f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.25f, 0.45f, 0.80f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.20f, 0.40f, 0.75f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.40f, 0.75f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.30f, 0.55f, 0.95f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.40f, 0.75f, 0.50f));

        if (ImGui::BeginMenu("Height Map")) {
            Spacing(2);

            ImGui::SliderInt("Resolution", &context.imageGenerationParameters.resolution, 16, 1024);

            //-Seeding ----------------------------------------------
            Spacing(2);
            ImGui::SeparatorText("Seeding");
            Spacing(1);
            
            ImGui::Checkbox("Random Seed", &context.imageGenerationParameters.isSeedRandom);
            ImGui::BeginDisabled(context.imageGenerationParameters.isSeedRandom);
            ImGui::InputInt("Seed", &context.imageGenerationParameters.noiseSeed);
            ImGui::EndDisabled();

            //-Mask ----------------------------------------------
            Spacing(2);
            ImGui::SeparatorText("Mask");
            Spacing(1);

            ImGui::Combo("Type", &context.imageGenerationParameters.mask.type, context.imageGenerationData.noiseListStr);

            Spacing(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleColor(6);

        //----------------------------------------------
        // COLOR MAP  (pink accent)
        //----------------------------------------------
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.25f, 0.30f, 0.55f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.95f, 0.45f, 0.80f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.90f, 0.40f, 0.75f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.90f, 0.40f, 0.75f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.55f, 0.95f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.90f, 0.40f, 0.75f, 0.50f));

        if (ImGui::BeginMenu("Color Map")) {
            Spacing(2);

            ImGui::Combo(
                "Color Map",
                &context.imageGenerationParameters.selectedColorMap,
                context.imageGenerationData.colorMaps,
                IM_ARRAYSIZE(context.imageGenerationData.colorMaps)
            );
            ImGui::Checkbox("Lerp", &context.imageGenerationParameters.colorMapLerp);

            Spacing(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleColor(6);

        //----------------------------------------------
        // NOISE  (red accent)
        //----------------------------------------------
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.25f, 0.0f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.95f, 0.0f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.90f, 0.0f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.90f, 0.0f, 0.75f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.90f, 0.0f, 0.0f, 0.50f));

        if (ImGui::BeginMenu("Noise")) {
            Spacing(2);

            ImGui::SliderFloat("Gaussian Sigma", &context.imageGenerationData.sigma, 0.01f, 1.f);

            // Noise stack
            Spacing(2);
            ImGui::SeparatorText("Noise Stack");
            Spacing(1);

            for (size_t i = 0; i < context.imageGenerationParameters.noiseStack.size(); i++) {
                Noise& noise = context.imageGenerationParameters.noiseStack[i];
                ImGui::PushID((int)i);

                //indent group
                ImGui::Indent(8.f);

                std::string label = "Layer " + std::to_string(i + 1);
                ImGui::Text(label.c_str());

                ImGui::Combo("Type", &noise.type, context.imageGenerationData.noiseListStr);
                ImGui::SliderInt("Octaves", &noise.nbOctave, 1, 8);
                ImGui::SliderFloat("Scale", &noise.scale, 0.01f, 10.f);

                if (ImGui::Button("Remove")) {
                    // https://en.cppreference.com/w/cpp/container/vector/erase
                    auto it = context.imageGenerationParameters.noiseStack.begin() + i; //auto because type is long af
                    context.imageGenerationParameters.noiseStack.erase(it);
                    i--;
                }

                ImGui::Unindent(8.f);
                ImGui::Separator();

                ImGui::PopID();
            }

            if (ImGui::Button("+ Add Noise Layer")) {
                Noise newNoise{};
                newNoise.type = NoiseType::PERLIN;
                context.imageGenerationParameters.noiseStack.push_back(newNoise);
            }
            
            Spacing(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleColor(6);

        //----------------------------------------------
        // OBJECTS  (green accent)
        //----------------------------------------------
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.10f, 0.40f, 0.20f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.18f, 0.60f, 0.32f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.14f, 0.52f, 0.26f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.14f, 0.52f, 0.26f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.22f, 0.75f, 0.40f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.14f, 0.52f, 0.26f, 0.50f));

        if (ImGui::BeginMenu("Objects")) {
            Spacing(2);

            ImGui::SliderFloat("Cone Scale", &context.coneScale, 0.01f, 1.0f);

            ImGui::SliderFloatRange2(
                "Height Spawn Range",
                &context.pointsGenerationParameters.heightTreshold.x,
                &context.pointsGenerationParameters.heightTreshold.y,
                0.0f, 1.0f
            );

            //-Poisson Disk Smapling --------------------------------------
            Spacing(2);
            ImGui::SeparatorText("Poisson Disk Sampling (PDS)");
            Spacing(1);

            ImGui::Checkbox("PDS Enabled", &context.pointsGenerationParameters.isPoissonEnabled);
            ImGui::SliderFloat("PDS Range", &context.pointsGenerationParameters.poissonRadius, 0.01f, 10.0f);

            //-Randomization ----------------------------------------------
            Spacing(2);
            ImGui::SeparatorText("Randomization");
            Spacing(1);

            ImGui::Checkbox("Random Rotations", &context.pointsGenerationParameters.isRotationRandom);
            if (context.pointsGenerationParameters.isRotationRandom) {
                ImGui::SliderFloat3(
                    "Rotation Offset",
                    (float*)&context.pointsGenerationParameters.rotationOffset, 0.f, 360.f
                );
            }

            Spacing(1);
            ImGui::Checkbox("Random Scale", &context.pointsGenerationParameters.isScaleRandom);
            if (context.pointsGenerationParameters.isScaleRandom) {
                ImGui::SliderFloat3(
                    "Scale Offset",
                    (float*)&context.pointsGenerationParameters.scaleOffset, 0.f, 10.f
                );
            }

            Spacing(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleColor(6);

        //----------------------------------------------
        // ACTIONS  (orange accent)
        //----------------------------------------------
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.55f, 0.30f, 0.05f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.80f, 0.48f, 0.10f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.70f, 0.40f, 0.08f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_Button,  ImVec4(0.55f, 0.28f, 0.05f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.42f, 0.10f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.55f, 0.15f, 1.f));

        if (ImGui::BeginMenu("Actions")) {
            Spacing(2);

            float btnWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::Button("Reload Color Map", ImVec2(btnWidth, 0)))
                generateHeightmap(context);

            Spacing(1);
            if (ImGui::Button("Regenerate Heightmap", ImVec2(btnWidth, 0))) {
                setupSeed(context);
                generateHeightmap(context);
            }

            Spacing(1);
            if (ImGui::Button("Regenerate Mesh", ImVec2(btnWidth, 0)))
                regenerateMeshFromImage(context);

            Spacing(1);
            if (ImGui::Button("Regenerate Objects", ImVec2(btnWidth, 0)))
                generateObjectsPositions(context);

            Spacing(2);
            ImGui::Separator();
            Spacing(2);

            //regen all bigger
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.38f, 0.05f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.52f, 0.12f, 1.f));
            if (ImGui::Button("Regenerate All", ImVec2(btnWidth, 36.f))) {
                setupSeed(context);
                generateHeightmap(context);
                regenerateMeshFromImage(context);
                generateObjectsPositions(context);
            }
            ImGui::PopStyleColor(2);

            Spacing(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleColor(6);

        ImGui::EndMainMenuBar();
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

    DrawFPS(10, 20);
}

