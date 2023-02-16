#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/Tema2/transform3D.h"
#include "trackGeometry.h"
#include "grassGeometry.h"
#include "enemyGenerator.h"

using namespace std;
using namespace m1;

#define Z_FAR		(200.f)
#define Z_NEAR		(.01f)
#define NO_ENEMY_CARS 7
#define START_TRIANGLE_IDX 1290
/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    // initializare factori scalare
    scaleX = 7;
    scaleY = 7;
    scaleZ = 7;

    // setare camera principala; e asezata in spatele masinii, putin mai sus
    camera = new implemented_camera::Camera();
    camera->Set(glm::vec3(initialTrajectoryVertices[0].x * 7, 2.2f, initialTrajectoryVertices[0].z * 7), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    // initializare vectori ai camerei ce vor fi utilizati pentru miniviewport 
    miniMapForward = glm::vec3(0, -1, 0);
    miniMapUp = camera->up;

    // coordonatele initiale ale masinii player-ului
    playerTranslateX = camera->GetTargetPosition().x;
    playerTranslateZ = camera->GetTargetPosition().z;
    // unghiul de rotatie initial al player-ului
    playerRotationAngle = atan2(camera->GetTargetPosition().x, camera->GetTargetPosition().z);

    // setare parametrii perspectiva ortografica
    right = 10.f;
    left = .01f;
    bottom = .01f;
    top = 10.f;

    // setare valori initiale viteza si rotatie camera
    float cameraSpeed = 5.4f;
    float rotationSpeed = 1.6f;

    // initializere contor lap curent
    currentLap = 1;
    insideFirstTriangle = false;

    // setare coordonata y player si inamici
    OYCoordinatePlayer = -0.05f;
    OYCoordinateEnemies = -0.1f;

    // initializare factor scalare vertex shader
    scaleFactor = 0.015f;

    // load mesh-uri
    glm::ivec2 resolution = window->GetResolution();
    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 30);

    Mesh* car0 = new Mesh("car5");
    car0->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "chick"), "Chick Hicks.obj");
    meshes[car0->GetMeshID()] = car0;
 
    Mesh* car1 = new Mesh("car6");
    car1->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "bernoulli"), "Francesco Bernoulli.obj");
    meshes[car1->GetMeshID()] = car1;
    
    Mesh* car2 = new Mesh("car3");
    car2->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "finn"), "Finn McMissile.obj");
    meshes[car2->GetMeshID()] = car2;
    
    Mesh* car3 = new Mesh("car4");
    car3->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "King"), "King.obj");
    meshes[car3->GetMeshID()] = car3;
    
    Mesh* car4 = new Mesh("car2");
    car4->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "Ramone"), "Ramone.obj");
    meshes[car4->GetMeshID()] = car4;

    Mesh* car5 = new Mesh("car0");
    car5->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "Mate"), "Mate.obj");
    meshes[car5->GetMeshID()] = car5;

    Mesh* car6 = new Mesh("car1");
    car6->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "Flo"), "Flo.obj");
    meshes[car6->GetMeshID()] = car6;

    Mesh* car7 = new Mesh("car7");
    car7->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "mcqueen"), "LightingMcqueen.obj");
    meshes[car7->GetMeshID()] = car7;

    Mesh* flag = new Mesh("flag");
    flag->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "flag"), "Flag.obj");
    meshes[flag->GetMeshID()] = flag;

    Mesh* track = track::CreateTrack("track", glm::vec3(0.1686f, 0.1764f, 0.1843f), initialTrajectoryVertices);
    AddMeshToList(track);

    // generare peisaj random
    int randomLandscape = rand() % 3;
    string treeName;
    glm::vec3 grassColor;
    if (randomLandscape == 0) {
        treeName = "PineSnow.obj";
        grassColor = NormalizedRGB(255, 250, 250);
    }
    if (randomLandscape == 1) {
        treeName = "Pine.obj";
        grassColor = NormalizedRGB(88, 161, 106);
    }
    if (randomLandscape == 2) {
        treeName = "Pine.obj";
        grassColor = NormalizedRGB(131, 79, 59);
    }

    Mesh* tree = new Mesh("tree");
    tree->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2", "tree"), treeName);
    meshes[tree->GetMeshID()] = tree;

    Mesh* grass = grass::CreateGrass("grass", grassColor);
    AddMeshToList(grass);

    // initializare shadere
    {
        Shader* shader = new Shader("DefaultColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "DefaultColorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("VertexColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexColorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    
    // setare rezolutie viewport
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    // generare puncte spawn copaci
    track::generateTreeSpawnPoints(initialTrajectoryVertices, treeSpawnPoints);
    // generare puncte spawn masini inamice
    enemyGenerator::generateEnemyCars(translateEnemy, currentEnemyPointIndex, timeElapsedBetweenPointsEnemy,
        initialTrajectoryVertices, enemyTrajectoryPoints, enemyRotation);
    // generare triunghiuri incadratoare pista
    trackTriangles = track::generateFewerTrackTriangles(initialTrajectoryVertices);
}


// randare masini inamice
void Tema2::renderEnemyCars()
{
    for (int i = 0; i < NO_ENEMY_CARS; i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(translateEnemy[i].x * scaleX, OYCoordinateEnemies, translateEnemy[i].z * scaleZ);
        modelMatrix *= transform3D::Scale(0.0047, 0.0047, 0.0047);
        modelMatrix *= transform3D::RotateOY(enemyRotation[i] + M_PI);

        if (i == 0) {
            RenderMesh(meshes["car0"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 1) {
            RenderMesh(meshes["car1"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 2) {
            RenderMesh(meshes["car2"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 3) {
            RenderMesh(meshes["car3"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 4) {
            RenderMesh(meshes["car4"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 5) {
            RenderMesh(meshes["car5"], shaders["DefaultColorShader"], modelMatrix);
        }
        if (i == 6) {
            RenderMesh(meshes["car6"], shaders["DefaultColorShader"], modelMatrix);
        }
    }
    
}


void Tema2::FrameStart()
{
    // stergere color buffer, setare cu culoarea noua si stergere depth buffer
    glClearColor(0.5411f, 0.8039f, 0.91764f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setare arie ecran de desenare
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


// randare scena
void Tema2::RenderScene(float deltaTimeSeconds) {
    // randare masina player
    modelMatrixPlayer = glm::mat4(1);
    modelMatrixPlayer *=  glm::translate(modelMatrixPlayer, glm::vec3(playerTranslateX, OYCoordinatePlayer, playerTranslateZ));
    modelMatrixPlayer *= transform3D::Scale(0.20, 0.20, 0.20);
    modelMatrixPlayer *= transform3D::RotateOY(playerRotationAngle + M_PI);
    RenderMesh(meshes["car7"], shaders["DefaultColorShader"], modelMatrixPlayer);

    // randare masini inamice
    renderEnemyCars();

    // randare steag finish
    modelMatrix = glm::mat4(1);
    modelMatrix *= glm::translate(modelMatrix, glm::vec3(trackTriangles[1290].x * scaleX - 0.8f, 0, trackTriangles[1290].z * scaleX ));
    modelMatrix *= transform3D::Scale(0.05, 0.05, 0.05);
    modelMatrix *= transform3D::RotateOY(M_PI);
    RenderMesh(meshes["flag"], shaders["DefaultColorShader"], modelMatrix);

    // randare copaci
    for (int i = 0; i < treeSpawnPoints.size(); i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(scaleX * treeSpawnPoints[i].x, -0.30f, scaleZ * treeSpawnPoints[i].z);
        modelMatrix *= transform3D::Scale(0.008f, 0.008f, 0.008f);
        modelMatrix *= transform3D::RotateOZ(M_PI / 2);
        modelMatrix *= transform3D::RotateOY(M_PI / 2);
        RenderMesh(meshes["tree"], shaders["DefaultColorShader"], modelMatrix);
    }

    // randare pista
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Scale(scaleX, 0, scaleZ);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, 0.0f);
    RenderMesh(meshes["track"], shaders["VertexColorShader"], modelMatrix);

    // randare iarba
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    modelMatrix *= transform3D::Translate(0, 0, -20);
    RenderMesh(meshes["grass"], shaders["VertexColorShader"], modelMatrix);
}


// update scena
void Tema2::Update(float deltaTimeSeconds)
{
    // afisare text lap curent
    std::stringstream str1;
    str1 << "LAP : " << currentLap;
    textRenderer->RenderText(str1.str(), 10, 10, 1.0f, kTextColor);

    // updatare pozitie masini inamice
    enemyGenerator::updateEnemyCars(deltaTimeSeconds, translateEnemy, currentEnemyPointIndex,
        timeElapsedBetweenPointsEnemy, enemyTrajectoryPoints, enemyRotation);
   
    // setare matrice proiectie in perspectiva pentru scena mare
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);
    // randare scena
    RenderScene(deltaTimeSeconds);

    // setare miniviewport
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // retinere vectori camera inainte de randare miniviewport in variabile auxiliare
    float prevDistanceToTarget = camera->distanceToTarget;
    glm::vec3 prevPosition = camera->position;
    glm::vec3 prevForward = camera->forward;
    glm::vec3 prevRight = camera->right;
    glm::vec3 prevUp = camera->up;
    // schimbare vectiru forward astfel incat sa se uite de sus la masina
    camera->forward = miniMapForward;
    // schimbare vector pozitie pt asezare deasupra camerei
    camera->position = glm::vec3(playerTranslateX, 20, playerTranslateZ);
    camera->up = miniMapUp;

    // setare proiectie ortografica
    projectionMatrix = glm::ortho(float (-miniViewportArea.width / 12), float(miniViewportArea.width / 12),
        float(-miniViewportArea.height / 12), float(miniViewportArea.height / 12), Z_NEAR, Z_FAR);
    // randare scena in miniviewport
    RenderScene(deltaTimeSeconds);
    // reintoarcere la valorile anterioare ale parametrilor camerei
    camera->distanceToTarget = prevDistanceToTarget;
    camera->position = prevPosition;
    camera->forward = prevForward;
    camera->right = prevRight;
    camera->up = prevUp;
}


void Tema2::FrameEnd()
{   
}


// randare obiecte
void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // trimitere date catre placa grafica utilizand shader-ul ales
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    // trimitere coordonata masinii jucatorului catre gpu
    int locationPlayerCoord = glGetUniformLocation(shader->program, "locationPlayerCoord");
    glUniform3fv(locationPlayerCoord, 1, glm::value_ptr(glm::vec3(playerTranslateX, OYCoordinatePlayer, playerTranslateZ)));
    // trimitere factor scalare catre gpu
    int locationScaleFactor = glGetUniformLocation(shader->program, "scaleFactor");
    glUniform1f(locationScaleFactor, scaleFactor);

    mesh->Render();
}


// primire input
void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // la apasarea space, masina incetineste
    if (window->KeyHold(GLFW_KEY_SPACE)) {
        if (cameraSpeed > 0) {
            cameraSpeed -= 0.02f;
        }
        if (rotationSpeed > 1.6f) {
            rotationSpeed -= 0.002f;
        }
    // pe shift are speed boost
    } else if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
        if (cameraSpeed < 12) {
            cameraSpeed += 0.04f;
        }
        if (rotationSpeed < 2.4f) {
            rotationSpeed += 0.004f;
        }
    }
    // altfel are viteza normala
    else {
        cameraSpeed = 5.4f;
        rotationSpeed = 1.6f;
    }

    
    // initial, consideram ca nu se ciocneste cu niciun inamic
    bool collision = false;
    // pentru fiecare inamic, verificam daca se ciocneste cu masina player-ului
    for (int i = 0; i < NO_ENEMY_CARS; i++) {
        float enemyX = translateEnemy[i].x * scaleX;
        float enemyZ = translateEnemy[i].z * scaleZ;
        // folosim formula pentru a afla daca se ciocnesc
        if (sqrt(pow((enemyX - camera->GetTargetPosition().x), 2) + pow((enemyZ - camera->GetTargetPosition().z), 2)) <= 1.02) {
            // daca se ciocneste cu o singura masina, iesim
            collision = true;
            break;
        }
    }

    // daca nu a avut loc nicio coliziune, putem misca masina
    if (collision == false) {
        // miscare inainte
        if (window->KeyHold(GLFW_KEY_W)) {
            // retinem valorile anterioare ale camerei
            glm::vec3 prevPositionLocal = camera->position;
            glm::vec3 prevForwardLocal = camera->forward;
            // aplicam translatarea in fata
            camera->TranslateForward(deltaTime * cameraSpeed);

            // pentru fiecare triunghi care formwaza pista, verificam daca noua pozitie a masinii e pe pista
            bool isInside = false;
            for (int i = 0; i < trackTriangles.size(); i += 3) {
                // folosim functia care returneaza daca un punct e intr-un triunghi sau nu
                if (track::checkInsideTriangle(trackTriangles[i] * scaleX,
                    trackTriangles[i + 1] * scaleX, trackTriangles[i + 2] * scaleX, 
                    camera->GetTargetPosition())) {
                    // daca e inauntru, putem continua
                    isInside = true;
                    // verificam daca se afla in triunghiul de start; daca da,
                    // atunci crestem current lap-ul
                    if (insideFirstTriangle == false && i == START_TRIANGLE_IDX) {
                        insideFirstTriangle = true;
                        currentLap++;
                    }
                    break;
                }
                if (insideFirstTriangle == true && i == START_TRIANGLE_IDX) {
                    insideFirstTriangle = false;
                }
            }
            // daca nu se afla pe pista, atunci revenim la valorile anterioare ale camerei
            if (isInside == false) {
                camera->position = prevPositionLocal;
                camera->forward = prevForwardLocal;
            }
        }

        // miscare inapoi
        if (window->KeyHold(GLFW_KEY_S)) {
            // retinem valorile anterioare ale camerei
            glm::vec3 prevPositionLocal = camera->position;
            glm::vec3 prevForwardLocal = camera->forward;
            // aplicam translatarea in spate
            camera->TranslateForward(-deltaTime * cameraSpeed);

            // pentru fiecare triunghi care formwaza pista, verificam daca noua pozitie a masinii e pe pista
            bool isInside = false;
            for (int i = 0; i < trackTriangles.size(); i += 3) {
                // folosim functia care returneaza daca un punct e intr-un triunghi sau nu
                if (track::checkInsideTriangle(trackTriangles[i] * scaleX,
                    trackTriangles[i + 1] * scaleX,
                    trackTriangles[i + 2] * scaleX, camera->GetTargetPosition())) {
                    // daca e inauntru, putem continua
                    isInside = true;
                    break;
                }
            }
            // daca nu se afla pe pista, atunci revenim la valorile anterioare ale camerei
            if (isInside == false) {
                camera->position = prevPositionLocal;
                camera->forward = prevForwardLocal;
            }
        }

        // rotire dreapta camera
        if (window->KeyHold(GLFW_KEY_D)) {
            // retinere valoare anterioara a vectorului forward
            glm::vec3 prevForward = camera->forward;
            // rotire camera
            camera->RotateThirdPerson_OY(-deltaTime * rotationSpeed);
            glm::vec3 currentForward = camera->forward;
            // calculul unghiului de rotatie al masinii folosind produsul scalar al
            // celor doua directii forward
            playerRotationAngle -= acos(glm::dot(prevForward, currentForward));

        }

        // rotire stanga camera
        if (window->KeyHold(GLFW_KEY_A)) {
            // retinere valoare anterioara a vectorului forward
            glm::vec3 prevForward = camera->forward;
            // rotire camera
            camera->RotateThirdPerson_OY(deltaTime * rotationSpeed);
            glm::vec3 currentForward = camera->forward;
            // calculul unghiului de rotatie al masinii folosind produsul scalar al
            // celor doua directii forward
            playerRotationAngle += acos(glm::dot(prevForward, currentForward));
        }
    }
    // coordonatele curente ale masinii player-ului
    playerTranslateX = camera->GetTargetPosition().x;
    playerTranslateZ = camera->GetTargetPosition().z;
}


void Tema2::OnKeyPress(int key, int mods)
{
    // la apasarea pe v, se schimba factorul de scalare
    // al varfurilor
    if (key == GLFW_KEY_V) {
        if (scaleFactor == 0.015f) {
            scaleFactor = -0.015f;
            OYCoordinateEnemies = 0.1f;
            OYCoordinatePlayer = 0.05f;
        }
        else {
            scaleFactor = 0.015f;
            OYCoordinatePlayer = -0.05f;
            OYCoordinateEnemies = -0.1f;
        }
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
