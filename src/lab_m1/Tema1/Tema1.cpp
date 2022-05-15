#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <random>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D22.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;           // logic x
    logicSpace.y = 0;           // logic y
    logicSpace.width = 160;     // logic width
    logicSpace.height = 90;     // logic height
    logicSpaceMinimap = logicSpace;
    logicSpaceMinimap.x -= 1.5f;
    logicSpaceMinimap.y -= 1.5f;
    logicSpaceMinimap.width += 2.5f;
    logicSpaceMinimap.height += 2.5f;
    resolution = window->GetResolution();

    centerLogicSpaceX = logicSpace.width / 2;
    centerLogicSpaceY = logicSpace.height / 2;
    viewScaleRatioX = resolution.x / logicSpace.width;
    viewScaleRatioY = resolution.y / logicSpace.height;

    Mesh* background = object2D::CreateRectangle("background", glm::vec3(0), logicSpace.width, logicSpace.height, glm::vec3(1, 1, 1), true);
    Mesh* healthbarStroke = object2D::CreateRectangle("healthbarStroke", glm::vec3(0), healthbarWidth, healthbarLength, glm::vec3(0.227f, 0.227f, 0.227f), false);
    Mesh* healthbar = object2D::CreateRectangle("healthbar", glm::vec3(0), healthbarWidth, healthbarLength, glm::vec3(0.850f, 0.321f, 0.368f), true);
    Mesh* circleBody = object2D::CreateCircle("circleBody", bodyRadius, circleNrTriangles, glm::vec3(0.850f, 0.321f, 0.368f));
    Mesh* circleHand = object2D::CreateCircle("circleHand", handsRadius, circleNrTriangles, glm::vec3(0.784f, 0.298f, 0.341f));
    Mesh* projectile = object2D::CreateSquare("projectile", glm::vec3(0), projectileLength, glm::vec3(0.227f, 0.227f, 0.227f), true);
    Mesh* enemyBody = object2D::CreateSquare("enemyBody", glm::vec3(0), enemyBodyLength, glm::vec3(0.019f, 0.949f, 0.686f), true);
    Mesh* enemyHand = object2D::CreateSquare("enemyHand", glm::vec3(0), enemyHandLength, glm::vec3(0.180f, 0.549f, 0.513f), true);
    Mesh* enemyCollisionCircle = object2D::CreateCircle("enemyCollisionCircle", enemyBodyLength / 2, circleNrTriangles, glm::vec3(0, 0, 1));
    Mesh* projectileCollisionCircle = object2D::CreateCircle("projectileCollisionCircle", projectileLength / 2, circleNrTriangles, glm::vec3(0, 1, 1));
    Mesh* rectangle = object2D::CreateRectangle("rectangle", glm::vec3(0), rectangleWidth, rectangleLength, glm::vec3(0.090f, 0.313f, 0.450f), true);
    Mesh* strokePlayerPosMinimap = object2D::CreateRectangle("strokePlayerPosMinimap", glm::vec3(0), logicSpace.width / 1.5f, logicSpace.height / 1.5f, glm::vec3(0.227f, 0.227f, 0.227f), false);
    Mesh* powerupHealth = object2D::CreateCircle("powerupHealth", powerupRadius, circleNrTriangles, glm::vec3(0, 0.003f, 0.050f));
    Mesh* powerupSpeed = object2D::CreateCircle("powerupSpeed", powerupRadius, circleNrTriangles, glm::vec3(0.850f, 0.564f, 0.211f));
    Mesh* powerupFirerate = object2D::CreateCircle("powerupFirerate", powerupRadius, circleNrTriangles, glm::vec3(0.949f, 0.337f, 0.113f));
    AddMeshToList(powerupSpeed);
    AddMeshToList(powerupFirerate);
    AddMeshToList(powerupHealth);
    AddMeshToList(strokePlayerPosMinimap);
    AddMeshToList(rectangle);
    AddMeshToList(projectileCollisionCircle);
    AddMeshToList(enemyCollisionCircle);
    AddMeshToList(background);
    AddMeshToList(healthbarStroke);
    AddMeshToList(healthbar);
    AddMeshToList(circleBody);
    AddMeshToList(circleHand);
    AddMeshToList(projectile);
    AddMeshToList(enemyBody);
    AddMeshToList(enemyHand);

    obstacles.emplace_back(Obstacle(10, 40, rectangleLength, rectangleWidth, 2.8f, 1.9f));
    obstacles.emplace_back(Obstacle(130, 60, rectangleLength, rectangleWidth, 1.5f, 1.3f));
    obstacles.emplace_back(Obstacle(100, 15, rectangleLength, rectangleWidth, 1.8f, 1.3f));
    obstacles.emplace_back(Obstacle(34, 45, rectangleLength, rectangleWidth, 1.1f, 2.1f));
    obstacles.emplace_back(Obstacle(130, 12, rectangleLength, rectangleWidth, 2.1f, 2.1f));
    obstacles.emplace_back(Obstacle(90, 24, rectangleLength, rectangleWidth, 1.2f, 1.4f));
    obstacles.emplace_back(Obstacle(110, 50, rectangleLength, rectangleWidth, 1.2f, 1.2f));
    obstacles.emplace_back(Obstacle(10, 70, rectangleLength, rectangleWidth, 1.2f, 1.2f));
    obstacles.emplace_back(Obstacle(80, 20, rectangleLength, rectangleWidth, 2.8f, 1.2f));
    obstacles.emplace_back(Obstacle(55, 20, rectangleLength, rectangleWidth, 1.8f, 1.2f));
    obstacles.emplace_back(Obstacle(50, 74, rectangleLength, rectangleWidth, 3.8f, 1.2f));
    obstacles.emplace_back(Obstacle(10, 10, rectangleLength, rectangleWidth, 3.8f, 1.4f));
    obstacles.emplace_back(Obstacle(110, 75, rectangleLength, rectangleWidth, 2.3f, 1.4f));


    offsetPlayerX = 0;
    offsetPlayerY = 0;

    enemyStartCounter = Engine::GetElapsedTime();
    powerupSpawnTime = enemyStartCounter;
    
    glm::ivec2 viewPort = glm::ivec2(resolution.x + resolution.x / 2, resolution.y + resolution.y / 2);

    viewSpace = ViewportSpace(-resolution.x / 4, -resolution.y / 4, viewPort.x, viewPort.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);
}


// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float inline Tema1::ComputeDistanceAB(float xA, float yA, float xB, float yB) {
    float deltaX = xB - xA;
    float deltaY = yB - yA;

    return glm::sqrt(deltaX * deltaX + deltaY * deltaY);
}

glm::vec4 Tema1::GenerateRectAttributes(float rangePosX, float rangePosY, float width, float height, float rangeScaleX, float rangeScaleY) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator

    std::uniform_int_distribution<> distrX(0, logicSpace.width); // define the range
    float genX = distrX(gen);
    while (genX > rangePosX && genX < logicSpace.width - rangePosX) {
        genX = distrX(gen);
    }

    std::uniform_int_distribution<> distrY(0, logicSpace.height); // define the range
    float genY = distrY(gen);
    while (genY > rangePosY && genY < logicSpace.height - rangePosY) {
        genY = distrY(gen);
    }

    std::uniform_int_distribution<> distrScaleX(0, width * rangeScaleX); // define the range
    float scaleX = distrScaleX(gen);

    std::uniform_int_distribution<> distrScaleY(0, height * rangeScaleY); // define the range
    float scaleY = distrScaleY(gen);

    return glm::vec4(genX, genY, scaleX, scaleY);
}

void inline Tema1::GenerateEnemy(float rangeXMin, float rangeXMax, float rangeYMin, float rangeYMax) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distrX(rangeXMin, rangeXMax); // define the range
    float genX = distrX(gen);

    std::uniform_int_distribution<> distrY(rangeYMin, rangeYMax); // define the range
    float genY = distrY(gen);
 
    float angle = glm::atan(genY + enemyBodyLength / 2 - centerLogicSpaceY + offsetPlayerY, genX + enemyBodyLength / 2 - centerLogicSpaceX + offsetPlayerX) - halfPI;

    enemies.emplace_back(Enemy(genX, genY, enemyBodyLength, angle, rand() % 30 + 15));
}

void inline Tema1::GenerateEnemyOffCenter(float offsetEnemySpawn) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distrX(0, logicSpace.width); // define the range
    float genX = distrX(gen);
    while (genX > offsetEnemySpawn && genX < logicSpace.width - offsetEnemySpawn) {
        genX = distrX(gen);
    }

    std::uniform_int_distribution<> distrY(0, logicSpace.height); // define the range
    float genY = distrY(gen);
    while (genY > offsetEnemySpawn / 2 && genX < logicSpace.height - offsetEnemySpawn / 2) {
        genY = distrY(gen);
    }

    float angle = glm::atan(genY + enemyBodyLength / 2 - centerLogicSpaceY + offsetPlayerY, genX + enemyBodyLength / 2 - centerLogicSpaceX + offsetPlayerX) - halfPI;

    enemies.emplace_back(Enemy(genX, genY, enemyBodyLength, angle, rand() % 30 + 15));
}

void inline Tema1::GeneratePowerup(float rangeXMin, float rangeXMax, float rangeYMin, float rangeYMax) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distrX(rangeXMin, rangeXMax); // define the range
    float genX = distrX(gen);

    std::uniform_int_distribution<> distrY(rangeYMin, rangeYMax); // define the range
    float genY = distrY(gen);

    std::uniform_int_distribution<> distrId(0, 2);
    int id = distrId(gen);
    string name;
    if (id == 0) {
        name = "powerupHealth";
    }
    else if (id == 1) {
        name = "powerupSpeed";
    } else {
        name = "powerupFirerate";
    }

    bool collision = true;

    while (collision) {
        collision = false;

        for (int i = 0; i < obstacles.size(); ++i) {
            if (CBColision(obstacles[i].x + obstacles[i].width / 2, obstacles[i].y + obstacles[i].height / 2, obstacles[i].width, obstacles[i].height, genX, genY, powerupRadius)) {
                genX = distrX(gen);
                genY = distrY(gen);
                break;
            }
        }
    }

    powerup.name = name;
    powerup.x = genX;
    powerup.y = genY;
}

bool inline Tema1::CBColision(float x1, float y1, float width, float height, float x2, float y2, float radius) {
    float circleDistanceX = glm::abs(x2 - x1);
    float circleDistanceY = glm::abs(y2 - y1);

    if (circleDistanceX > (width / 2 + radius)) return false;
    if (circleDistanceY > (height / 2 + radius)) return false;

    if (circleDistanceX <= (width / 2)) return true;
    if (circleDistanceY <= (height / 2)) return true;

    float cornerDistanceSquared = (circleDistanceX - width / 2) * (circleDistanceX - width / 2) +
        (circleDistanceY - height / 2) * (circleDistanceY - height / 2);

    return cornerDistanceSquared <= (radius * radius);
}

bool inline Tema1::CCColision(float x1, float y1, float radius1, float x2, float y2, float radius2) {
    return ((x2 - x1) * (x2 - x1) + (y1 - y2) * (y1 - y2)) <= ((radius1 + radius2) * (radius1 + radius2));
}

void Tema1::Update(float deltaTimeSeconds)
{
    // Checks for game over
    if (health <= 0) {
        printf("\nDeath comes to us all...seems like it came to you too...\nStart the game again for another try.\n\n");
        window->Close();
        return;
    }

    float currTime = Engine::GetElapsedTime();

    // Checks powerups if are applied
    if (isSpeedup && currTime - powreupActivatedTime >= powerupActiveTime) {
        isSpeedup = false;
        playerSpeed -= 10;
    }
    else if (isFirerate && currTime - powreupActivatedTime >= powerupActiveTime) {
        isFirerate = false;
        projectileMaxTime += 0.5f;
    }

    // Despawn powerups if they were too long on the screen or the player grabbed it
    if (totalPowerupsSpawned == 1) {
        if (currTime - powerupSpawnTime >= powerupTime) {
            totalPowerupsSpawned = 0;
            powerupSpawnTime = currTime;
        }
        else {
            if (CCColision(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY, bodyRadius, powerup.x + powerupRadius, powerup.y + powerupRadius, powerupRadius)) {
                totalPowerupsSpawned = 0;
                powerupSpawnTime = currTime;
                if (powerup.name == "powerupHealth") {
                    health += health * 0.5f;
                    if (health > 100) {
                        health = 100;
                    }
                }
                else if (powerup.name == "powerupSpeed") {
                    playerSpeed += 10;
                    isSpeedup = true;
                    powreupActivatedTime = currTime;
                }
                else {
                    projectileMaxTime -= 0.5f;
                    isFirerate = true;
                    powreupActivatedTime = currTime;
                }
            }
        }
    }

    // Generate powerup after timer
    if (totalPowerupsSpawned == 0 && currTime - powerupSpawnTime >= powerupCooldown) {
        if (centerLogicSpaceX + offsetPlayerX >= centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY >= centerLogicSpaceY) {
            GeneratePowerup(powerupRadius, centerLogicSpaceX, powerupRadius, centerLogicSpaceY);
        }
        else if (centerLogicSpaceX + offsetPlayerX < centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY > centerLogicSpaceY) {
            GeneratePowerup(centerLogicSpaceX, logicSpace.width - powerupRadius, powerupRadius, centerLogicSpaceY);
        }
        else if (centerLogicSpaceX + offsetPlayerX < centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY < centerLogicSpaceY) {
            GeneratePowerup(centerLogicSpaceX, logicSpace.width - powerupRadius, centerLogicSpaceY, logicSpace.height - powerupRadius);
        }
        else {
            GeneratePowerup(powerupRadius, centerLogicSpaceX, centerLogicSpaceY, logicSpace.height - powerupRadius);
        }
        
        ++totalPowerupsSpawned;
        powerupSpawnTime = currTime;
    }

    // Firerate for player
    if (isPlayerShoting) {
        if (currTime - startCounter > projectileMaxTime) {
            startCounter = -1;
            isPlayerShoting = false;
        }
    }

    // Enemy - projectiles collision
    for (int i = 0; i < enemies.size(); ++i) {
        for (int j = 0; j < projectiles.size(); ++j) {
            if (CCColision(projectiles[j].x, projectiles[j].y, projectiles[j].length / 2, enemies[i].x + enemies[i].length / 2, enemies[i].y + enemies[i].length / 2, enemies[i].length / 2)) {
                if (health <= 95) {
                    health += 5;
                }
                else {
                    health += 100 - health;
                }
                ++score;
                printf("Enemy killed! Score is: %d\n", score);
                enemiesToBeErased.push_back(i);
                projectilesToBeErased.push_back(j);
            }
        }
    }

    // Erase enemies after collision
    for (int i = 0; i < enemiesToBeErased.size(); ++i) {
        enemies.erase(enemies.begin() + enemiesToBeErased[i]);
    }

    // Erase projectiles after collision
    for (int i = 0; i < projectilesToBeErased.size(); ++i) {
        projectiles.erase(projectiles.begin() + projectilesToBeErased[i]);
    }

    // Clear vector of enemies to be erased
    if (!enemiesToBeErased.empty())
        enemiesToBeErased.clear();

    // Clear vector of projectiles to be erased
    if (!projectilesToBeErased.empty())
        projectilesToBeErased.clear();

    // Enemy - player collision
    for (int i = 0; i < enemies.size(); ++i) {
        if (CCColision(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY, bodyRadius, enemies[i].x + enemies[i].length / 2, enemies[i].y + enemies[i].length / 2, enemies[i].length / 2)) {
            health -= 10;
            enemiesToBeErased.push_back(i);
        }
    }

    // Erase enemy after collision
    for (int i = 0; i < enemiesToBeErased.size(); ++i) {
        enemies.erase(enemies.begin() + enemiesToBeErased[i]);
    }

    // Clear vector of enemies to be erased
    if (!enemiesToBeErased.empty())
        enemiesToBeErased.clear();

    // Erase projectile if its distance to the original spawn is too big
    if (!projectiles.empty() && ComputeDistanceAB(projectiles[0].startX, projectiles[0].startY, projectiles[0].x, projectiles[0].y) >= projectileMaxDist) {
        projectiles.erase(projectiles.begin());
    }

    // Projectile - obstacles collision
    for (int i = 0; i < projectiles.size(); ++i) {
        for (int j = 0; j < obstacles.size(); ++j) {
            if (CBColision(obstacles[j].x + obstacles[j].width / 2, obstacles[j].y + obstacles[j].height / 2, obstacles[j].width, obstacles[j].height, projectiles[i].x, projectiles[i].y, projectiles[i].length / 2)) {
                projectilesToBeErased.push_back(i);
            }
        }
    }

    // Erase projectiles after collision
    for (int i = 0; i < projectilesToBeErased.size(); ++i) {
        projectiles.erase(projectiles.begin() + projectilesToBeErased[i]);
    }

    // Clear vector of projectiles to be erased
    if (!projectilesToBeErased.empty())
        projectilesToBeErased.clear();

    // Update projectiles position
    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i].x += glm::sin(projectiles[i].angle) * projectiles[i].speed * deltaTimeSeconds;
        projectiles[i].y -= glm::cos(projectiles[i].angle) * projectiles[i].speed * deltaTimeSeconds;
    }

    // Generate enemies after a counter
    if (enemies.size() < 4 && currTime - enemyStartCounter > enemyMinTime) {
        if (centerLogicSpaceX + offsetPlayerX <= centerLogicSpaceX + offsetEnemySpawn && centerLogicSpaceY + offsetPlayerY <= centerLogicSpaceY + offsetEnemySpawn / 2 &&
            centerLogicSpaceX + offsetPlayerX > centerLogicSpaceX - offsetEnemySpawn && centerLogicSpaceY + offsetPlayerY > centerLogicSpaceY - offsetEnemySpawn / 2) {
            GenerateEnemyOffCenter(offsetEnemySpawn);
        }
        else if (centerLogicSpaceX + offsetPlayerX >= centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY >= centerLogicSpaceY) {
            GenerateEnemy(0, centerLogicSpaceX, 0, centerLogicSpaceY);
        }
        else if (centerLogicSpaceX + offsetPlayerX < centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY > centerLogicSpaceY) {
            GenerateEnemy(centerLogicSpaceX, logicSpace.width, 0, centerLogicSpaceY);
        }
        else if (centerLogicSpaceX + offsetPlayerX < centerLogicSpaceX && centerLogicSpaceY + offsetPlayerY < centerLogicSpaceY) {
            GenerateEnemy(centerLogicSpaceX, logicSpace.width, centerLogicSpaceY, logicSpace.height);
        }
        else {
            GenerateEnemy(0, centerLogicSpaceX, centerLogicSpaceY, logicSpace.height);
        }
        enemyStartCounter = currTime;
    }

    // Update enemies position and angle
    for (int i = 0; i < enemies.size(); ++i) {
        enemies[i].angle = glm::atan(enemies[i].y + enemyBodyLength / 2 - centerLogicSpaceY - offsetPlayerY, enemies[i].x + enemyBodyLength / 2 - centerLogicSpaceX - offsetPlayerX) - halfPI;
        enemies[i].x += glm::sin(enemies[i].angle) * enemies[i].speed * deltaTimeSeconds;
        enemies[i].y -= glm::cos(enemies[i].angle) * enemies[i].speed * deltaTimeSeconds;
    }

    // Draw screen
    SetViewportArea(viewSpace, glm::vec3(0), true);

    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
    DrawScene(visMatrix);

    // Draw MiniMap
    isMinimap = true;
    ViewportSpace viewPortMinimap = ViewportSpace(0, 0, resolution.x / 5, resolution.y / 5);
    SetViewportArea(viewPortMinimap, glm::vec3(0), true);

    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2D(logicSpaceMinimap, viewPortMinimap);
    DrawScene(visMatrix);
    isMinimap = false;
}


void Tema1::FrameEnd()
{
}


void Tema1::DrawScene(glm::mat3 visMatrix)
{
    if (!isMinimap) {
        modelMatrix = visMatrix *
            transform2D::Translate(logicSpace.width - logicSpace.width / 6 + offsetPlayerX, logicSpace.height - logicSpace.height / 6 + offsetPlayerY) *
            transform2D::Scale(1.f * health / 100, 1) *
            transform2D::Rotate(glm::radians(180.f));
        RenderMesh2D(meshes["healthbar"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(logicSpace.width - healthbarWidth - logicSpace.width / 6 + offsetPlayerX, logicSpace.height - healthbarLength - logicSpace.height / 6 + offsetPlayerY);
        RenderMesh2D(meshes["healthbarStroke"], shaders["VertexColor"], modelMatrix);
    }
    else {
        modelMatrix = visMatrix * transform2D::Translate(logicSpace.width / 6 + offsetPlayerX, logicSpace.height / 6 + offsetPlayerY);
        RenderMesh2D(meshes["strokePlayerPosMinimap"], shaders["VertexColor"], modelMatrix);
    }


    if (totalPowerupsSpawned == 1) {
        modelMatrix = visMatrix * transform2D::Translate(powerup.x, powerup.y);
        RenderMesh2D(meshes[powerup.name], shaders["VertexColor"], modelMatrix);
    }

    if (isDebugOn) {
        for (int i = 0; i < enemies.size(); ++i) {
            modelMatrix = visMatrix * transform2D::Translate(enemies[i].x + enemies[i].length / 2, enemies[i].y + enemies[i].length / 2);
            RenderMesh2D(meshes["enemyCollisionCircle"], shaders["VertexColor"], modelMatrix);
        }

        for (int i = 0; i < projectiles.size(); ++i) {
            modelMatrix = visMatrix * transform2D::Translate(projectiles[i].x, projectiles[i].y);
            RenderMesh2D(meshes["projectileCollisionCircle"], shaders["VertexColor"], modelMatrix);
        }
    }

    for (int i = 0; i < enemies.size(); ++i) {
        modelMatrix = visMatrix * transform2D::Translate(enemies[i].x + enemyBodyLength / 2, enemies[i].y + enemyBodyLength / 2) * transform2D::Rotate(enemies[i].angle) * transform2D::Translate(-enemyBodyLength / 2, -enemyBodyLength / 2 - enemyHandLength / 2);
        RenderMesh2D(meshes["enemyHand"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(enemies[i].x + enemyBodyLength / 2, enemies[i].y + enemyBodyLength / 2) * transform2D::Rotate(enemies[i].angle) * transform2D::Translate(enemyBodyLength - enemyHandLength - enemyBodyLength / 2, -enemyBodyLength / 2 - enemyHandLength / 2);
        RenderMesh2D(meshes["enemyHand"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(enemies[i].x + enemies[i].length / 2, enemies[i].y + enemies[i].length / 2) * transform2D::Rotate(enemies[i].angle) * transform2D::Translate(-enemies[i].length / 2, -enemies[i].length / 2);
        RenderMesh2D(meshes["enemyBody"], shaders["VertexColor"], modelMatrix);
    }

    for (int i = 0; i < obstacles.size(); ++i) {
        modelMatrix = visMatrix * transform2D::Translate(obstacles[i].x, obstacles[i].y) * transform2D::Scale(obstacles[i].scaleX, obstacles[i].scaleY);
        RenderMesh2D(meshes["rectangle"], shaders["VertexColor"], modelMatrix);
    }

    for (int i = 0; i < projectiles.size(); ++i) {
        modelMatrix = visMatrix * transform2D::Translate(projectiles[i].x, projectiles[i].y) * transform2D::Rotate(projectiles[i].angle) * transform2D::Translate(-halfProjectileLength, -halfProjectileLength);
        RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
    }

    modelMatrix = visMatrix * transform2D::Translate(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY) * transform2D::Rotate(rotationAnglePlayer) * transform2D::Translate(offsetHandsX, -offsetHandsY);
    RenderMesh2D(meshes["circleHand"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY) * transform2D::Rotate(rotationAnglePlayer) * transform2D::Translate(-offsetHandsX, -offsetHandsY);
    RenderMesh2D(meshes["circleHand"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY) * transform2D::Rotate(rotationAnglePlayer);
    RenderMesh2D(meshes["circleBody"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix;
    RenderMesh2D(meshes["background"], shaders["VertexColor"], modelMatrix);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
        // Checks for player - outside walls collisions and player - obstacles collisions
        if (window->KeyHold(GLFW_KEY_W)) {
            if (centerLogicSpaceY + offsetPlayerY + bodyRadius < logicSpace.height) {
                bool collision = false;
                float tmp = playerSpeed * deltaTime;
                for (int i = 0; i < obstacles.size(); ++i) {
                    if (CBColision(obstacles[i].x + obstacles[i].width / 2, obstacles[i].y + obstacles[i].height / 2, obstacles[i].width, obstacles[i].height, centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY + tmp, bodyRadius)) {
                        collision = true;
                        break;
                    }
                }
                if (!collision) {
                    logicSpace.y += tmp;
                    offsetPlayerY += tmp;
                }
            }
        }
        else if (window->KeyHold(GLFW_KEY_S)) {
            if (centerLogicSpaceY + offsetPlayerY - bodyRadius > 0) {
                bool collision = false;
                float tmp = playerSpeed * deltaTime;
                for (int i = 0; i < obstacles.size(); ++i) {
                    if (CBColision(obstacles[i].x + obstacles[i].width / 2, obstacles[i].y + obstacles[i].height / 2, obstacles[i].width, obstacles[i].height, centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY - tmp, bodyRadius)) {
                        collision = true;
                        break;
                    }
                }
                if (!collision) {
                    logicSpace.y -= tmp;
                    offsetPlayerY -= tmp;
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            bool collision = false;
            if (centerLogicSpaceX + offsetPlayerX - bodyRadius > 0) {
                bool collision = false;
                float tmp = playerSpeed * deltaTime;
                for (int i = 0; i < obstacles.size(); ++i) {
                    if (CBColision(obstacles[i].x + obstacles[i].width / 2, obstacles[i].y + obstacles[i].height / 2, obstacles[i].width, obstacles[i].height, centerLogicSpaceX + offsetPlayerX - tmp, centerLogicSpaceY + offsetPlayerY, bodyRadius)) {
                        collision = true;
                        break;
                    }
                }
                if (!collision) {
                    logicSpace.x -= tmp;
                    offsetPlayerX -= tmp;
                }
            }
        }
        else if (window->KeyHold(GLFW_KEY_D)) {
            bool collision = false;
            float tmp = playerSpeed * deltaTime;
            if (centerLogicSpaceX + offsetPlayerX + bodyRadius < logicSpace.width) {
                for (int i = 0; i < obstacles.size(); ++i) {
                    if (CBColision(obstacles[i].x + obstacles[i].width / 2, obstacles[i].y + obstacles[i].height / 2, obstacles[i].width, obstacles[i].height, centerLogicSpaceX + offsetPlayerX + tmp, centerLogicSpaceY + offsetPlayerY, bodyRadius)) {
                        collision = true;
                        break;
                    }
                }
                if (!collision) {
                    logicSpace.x += tmp;
                    offsetPlayerX += tmp;
                }
            }
        }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Enables debug mode for collisions
    if (key == GLFW_KEY_1) {
        isDebugOn = !isDebugOn;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float newMouseX = mouseX / viewScaleRatioX;
    float newMouseY = resolution.y - mouseY;
    newMouseY = newMouseY / viewScaleRatioY;

    rotationAnglePlayer = glm::atan(centerLogicSpaceY - newMouseY, centerLogicSpaceX - newMouseX) - halfPI;
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_2 && startCounter == -1) {
        if (!isPlayerShoting) {
            projectiles.emplace_back(Projectile(centerLogicSpaceX + offsetPlayerX, centerLogicSpaceY + offsetPlayerY, projectileLength, rotationAnglePlayer, projectileSpeed));
            rotationAngleProj = rotationAnglePlayer;
        }
        isPlayerShoting = true;
        startCounter = Engine::GetElapsedTime();
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
