#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera.h"
#include <components/text_renderer.h>

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void renderEnemyCars();
        void RenderScene(float deltaTimeSeconds);

     protected:
        glm::mat4 modelMatrix;
        float scaleX, scaleY, scaleZ;
        ViewportArea miniViewportArea;
        float OYCoordinatePlayer;
        float OYCoordinateEnemies;
        float scaleFactor;
        std::vector<glm::vec3> treeSpawnPoints;
        std::unordered_map<int, std::vector<glm::vec3>> enemyTrajectoryPoints;
        std::vector <float> enemyRotation;
        std::vector <int> currentEnemyPointIndex;
        std::vector <float> timeElapsedBetweenPointsEnemy;
        std::vector<glm::vec3> translateEnemy;
        float playerRotationRadius = 0;
        float playerRotationAngle = 0;
        float playerTranslateX = 0;
        float playerTranslateZ = 0;
        glm::mat4 modelMatrixPlayer;
        std::vector<glm::vec3> trackTriangles;

        implemented_camera::Camera* camera;

        glm::vec3 miniMapForward;
        glm::vec3 miniMapUp;

        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        bool projectionType;

        GLfloat right;
        GLfloat left;
        GLfloat bottom;
        GLfloat top;

        int currentLap;
        bool insideFirstTriangle;

        gfxc::TextRenderer* textRenderer;
        const glm::vec3 kTextColor = glm::vec3(0.1686f, 0.1764f, 0.1843f);
        gfxc::TextRenderer* textRenderer2;
        float cameraSpeed;
        float rotationSpeed;

        std::vector<glm::vec3> initialTrajectoryVertices = {
            glm::vec3(0.71, 0, 2.86),//a

            glm::vec3(1.72, 0, 3.24),//b

            glm::vec3(2.81, 0, 3.15),//c

            glm::vec3(3.67, 0, 2.48),//d

            glm::vec3(4.5, 0, 1.78),//e

            glm::vec3(5.54, 0, 2.07),//f

            glm::vec3(6.62, 0, 2.16),//g

            glm::vec3(7.33, 0, 1.32),//h

            glm::vec3(6.84, 0, 0.37),//i

            glm::vec3(5.77, 0, 0.2),//j

            glm::vec3(4.7, 0, 0.22),//k

            glm::vec3(3.74, 0, 0.7),//l

            glm::vec3(2.89, 0, 1.35),//m

            glm::vec3(1.83, 0, 1.47),//n

            glm::vec3(2.22, 0, 0.47),//c1

            glm::vec3(3.03, 0, -0.23),//d1

            glm::vec3(2.92, 0, -1.28),//e1

            glm::vec3(1.98, 0, -1.74),//g1

            glm::vec3(1.24, 0, -1.01),//h1

            glm::vec3(0.86, 0, -0.05),//i1

            glm::vec3(0.79, 0, 0.99),//o

            glm::vec3(0.25, 0, 1.87),//p

            // pt calcul mai adaug iar primul
            glm::vec3(0.71, 0, 2.86),//a
        };

    };
}   // namespace m1

