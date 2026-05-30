#include "game.h"

void UpdatePlayer(Player* ship)
{
    if (IsKeyDown(KEY_RIGHT))
        ship->rotation += 5.0f;
    if (IsKeyDown(KEY_LEFT))
        ship->rotation -= 5.0f;

    if (IsKeyDown(KEY_UP)) {
        float rotationInRadians = (ship->rotation - 90) * DEG2RAD;
        ship->velocity.x += cosf(rotationInRadians) * 0.15f;
        ship->velocity.y += sinf(rotationInRadians) * 0.15f;
    }

    ship->velocity.x *= 0.99f;
    ship->velocity.y *= 0.99f;

    float maxSpeed = 6.0f;
    float currentSpeed = sqrtf(ship->velocity.x * ship->velocity.x + ship->velocity.y * ship->velocity.y);

    if (currentSpeed > maxSpeed) {
        ship->velocity.x = (ship->velocity.x / currentSpeed) * maxSpeed;
        ship->velocity.y = (ship->velocity.y / currentSpeed) * maxSpeed;
    }

    ship->position.x += ship->velocity.x;
    ship->position.y += ship->velocity.y;

    if (ship->position.x > screenWidth + ship->size)
        ship->position.x = -ship->size;
    else if (ship->position.x < -ship->size)
        ship->position.x = screenWidth + ship->size;

    if (ship->position.y > screenHeight + ship->size)
        ship->position.y = -ship->size;
    else if (ship->position.y < -ship->size)
        ship->position.y = screenHeight + ship->size;
}

void UpdateBoss(Boss* boss, Player* ship, Bullet** bulletsHead, bool isGameOver)
{
    if (!boss->active)
        return;

    if (boss->isCrossing) {
        boss->position.x += boss->velocity.x;

        if (boss->velocity.x > 0 && boss->position.x >= screenWidth - 100) {
            boss->position.x = screenWidth - 100;
            boss->isCrossing = false;
            boss->bounceCount = 0;
            boss->velocity.x = 0;
        } else if (boss->velocity.x < 0 && boss->position.x <= 100) {
            boss->position.x = 100;
            boss->isCrossing = false;
            boss->bounceCount = 0;
            boss->velocity.x = 0;
        }
    } else {
        boss->position.y += boss->velocity.y;
        bool bounced = false;

        if (boss->position.y < boss->radius) {
            boss->position.y = boss->radius;
            boss->velocity.y *= -1;
            bounced = true;
        } else if (boss->position.y > screenHeight - boss->radius) {
            boss->position.y = screenHeight - boss->radius;
            boss->velocity.y *= -1;
            bounced = true;
        }

        if (bounced) {
            boss->bounceCount++;
            if (boss->bounceCount >= 5) {
                boss->isCrossing = true;
                if (boss->position.x > screenWidth / 2.0f) {
                    boss->velocity.x = -4.0f;
                } else {
                    boss->velocity.x = 4.0f;
                }
            }
        }
    }

    if (!isGameOver && !boss->isCrossing) {
        boss->shootTimer -= GetFrameTime();
        if (boss->shootTimer <= 0.0f) {
            for (int i = -1; i <= 1; i++) {
                Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));
                newBullet->position = boss->position;

                float angle = atan2f(ship->position.y - boss->position.y, ship->position.x - boss->position.x) + (i * 0.2f);
                float speed = 5.0f;

                newBullet->velocity.x = cosf(angle) * speed;
                newBullet->velocity.y = sinf(angle) * speed;
                newBullet->lifeTime = 2.0f;
                newBullet->isEnemy = true;
                newBullet->next = *bulletsHead;
                *bulletsHead = newBullet;
            }
            boss->shootTimer = 1.5f;
        }
    }
}

void UpdateEnemy(Enemy* ufo, Player* ship, Bullet** bulletsHead, Asteroid* asteroids, bool isGameOver)
{
    if (!ufo->active)
        return;

    if (!isGameOver) {
        if (ship->position.y > ufo->position.y)
            ufo->velocity.y += 0.03f;
        else
            ufo->velocity.y -= 0.03f;

        if (ufo->velocity.y > 1.5f)
            ufo->velocity.y = 1.5f;
        if (ufo->velocity.y < -1.5f)
            ufo->velocity.y = -1.5f;
    }

    ufo->position.x += ufo->velocity.x;
    ufo->position.y += ufo->velocity.y;

    if (ufo->position.x > screenWidth + ufo->radius)
        ufo->position.x = -ufo->radius;
    else if (ufo->position.x < -ufo->radius)
        ufo->position.x = screenWidth + ufo->radius;
    if (ufo->position.y > screenHeight + ufo->radius)
        ufo->position.y = -ufo->radius;
    else if (ufo->position.y < -ufo->radius)
        ufo->position.y = screenHeight + ufo->radius;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            if (CheckCollisionCircles(ufo->position, ufo->radius, asteroids[i].position, asteroids[i].radius)) {
                ufo->active = false;
                break;
            }
        }
    }

    if (!ufo->active)
        return;

    if (!isGameOver) {
        ufo->shootTimer -= GetFrameTime();
        if (ufo->shootTimer <= 0.0f) {
            float dx = ship->position.x - ufo->position.x;
            float dy = ship->position.y - ufo->position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance > 0) {
                Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));
                newBullet->position = ufo->position;

                float speed = 6.0f;
                newBullet->velocity.x = (dx / distance) * speed;
                newBullet->velocity.y = (dy / distance) * speed;
                newBullet->lifeTime = 1.5f;
                newBullet->isEnemy = true;
                newBullet->next = *bulletsHead;
                *bulletsHead = newBullet;
            }
            ufo->shootTimer = GetRandomValue(150, 300) / 100.0f;
        }
    }
}

void UpdateBullets(Bullet** bulletsHead, Player* ship, float* shootCooldown, Asteroid* asteroids, Enemy* ufo, Boss* boss, int* score, bool isGameOver)
{
    float baseAngle = ship->rotation - 90.0f;

    if (*shootCooldown > 0.0f)
        *shootCooldown -= GetFrameTime();

    if (!isGameOver && IsKeyDown(KEY_SPACE) && *shootCooldown <= 0.0f) {
        Bullet* newBullet = (Bullet*)malloc(sizeof(Bullet));

        newBullet->position.x = ship->position.x + cosf(baseAngle * DEG2RAD) * ship->size;
        newBullet->position.y = ship->position.y + sinf(baseAngle * DEG2RAD) * ship->size;

        float bulletSpeed = 10.0f;
        newBullet->velocity.x = cosf(baseAngle * DEG2RAD) * bulletSpeed;
        newBullet->velocity.y = sinf(baseAngle * DEG2RAD) * bulletSpeed;
        newBullet->lifeTime = 0.9f;
        newBullet->isEnemy = false;
        newBullet->next = *bulletsHead;
        *bulletsHead = newBullet;
        *shootCooldown = 0.2f;
    }

    Bullet* currentBullet = *bulletsHead;
    Bullet* previousBullet = NULL;

    while (currentBullet != NULL) {
        currentBullet->position.x += currentBullet->velocity.x;
        currentBullet->position.y += currentBullet->velocity.y;
        currentBullet->lifeTime -= GetFrameTime();

        if (currentBullet->position.x > screenWidth)
            currentBullet->position.x = 0;
        else if (currentBullet->position.x < 0)
            currentBullet->position.x = screenWidth;
        if (currentBullet->position.y > screenHeight)
            currentBullet->position.y = 0;
        else if (currentBullet->position.y < 0)
            currentBullet->position.y = screenHeight;

        bool bulletHit = false;

        if (!currentBullet->isEnemy) {
            if (boss->active && CheckCollisionCircles(currentBullet->position, 2.0f, boss->position, boss->radius)) {
                bulletHit = true;
                boss->health--;
                *score += 20;

                if (boss->health <= 0) {
                    boss->active = false;
                    *score += 2000;
                }
            }

            if (!bulletHit && ufo->active && CheckCollisionCircles(currentBullet->position, 2.0f, ufo->position, ufo->radius)) {
                bulletHit = true;
                ufo->active = false;
                *score += 500;
            }
        }

        if (!bulletHit) {
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active) {
                    if (CheckCollisionCircles(currentBullet->position, 2.0f, asteroids[i].position, asteroids[i].radius)) {
                        bulletHit = true;

                        if (!currentBullet->isEnemy) {
                            asteroids[i].active = false;

                            if (asteroids[i].radius >= 40.0f)
                                *score += 50;
                            else
                                *score += 100;

                            if (asteroids[i].radius >= 20.0f) {
                                float newRadius = asteroids[i].radius / 2.0f;
                                int spawned = 0;

                                for (int j = 0; j < MAX_ASTEROIDS && spawned < 2; j++) {
                                    if (!asteroids[j].active) {
                                        asteroids[j].active = true;
                                        float offset = (spawned == 0) ? -newRadius : newRadius;
                                        asteroids[j].position.x = asteroids[i].position.x + offset;
                                        asteroids[j].position.y = asteroids[i].position.y + offset;
                                        asteroids[j].radius = newRadius;
                                        for (int v = 0; v < ASTEROID_VERTICES; v++) {
                                            asteroids[j].vertexOffsets[v] = GetRandomValue(80, 120) / 100.0f;
                                        }
                                        asteroids[j].velocity.x = GetRandomValue(-300, 300) / 100.0f;
                                        asteroids[j].velocity.y = GetRandomValue(-300, 300) / 100.0f;
                                        spawned++;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        if (bulletHit)
            currentBullet->lifeTime = 0.0f;

        if (currentBullet->lifeTime <= 0.0f) {
            Bullet* toDelete = currentBullet;
            if (previousBullet == NULL) {
                *bulletsHead = currentBullet->next;
                currentBullet = *bulletsHead;
            } else {
                previousBullet->next = currentBullet->next;
                currentBullet = previousBullet->next;
            }
            free(toDelete);
        } else {
            previousBullet = currentBullet;
            currentBullet = currentBullet->next;
        }
    }
}

void UpdateAsteroids(Asteroid* asteroids)
{
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active)
            continue;

        for (int j = i + 1; j < MAX_ASTEROIDS; j++) {
            if (!asteroids[j].active)
                continue;

            float dx = asteroids[i].position.x - asteroids[j].position.x;
            float dy = asteroids[i].position.y - asteroids[j].position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance == 0.0f) {
                dx = 0.1f;
                distance = 0.1f;
            }

            float sumRadii = asteroids[i].radius + asteroids[j].radius;
            if (distance < sumRadii) {
                float nx = dx / distance;
                float ny = dy / distance;

                float overlap = sumRadii - distance;
                asteroids[i].position.x += nx * (overlap / 2.0f);
                asteroids[i].position.y += ny * (overlap / 2.0f);
                asteroids[j].position.x -= nx * (overlap / 2.0f);
                asteroids[j].position.y -= ny * (overlap / 2.0f);

                float dvx = asteroids[i].velocity.x - asteroids[j].velocity.x;
                float dvy = asteroids[i].velocity.y - asteroids[j].velocity.y;

                float dotProduct = (dvx * nx) + (dvy * ny);

                if (dotProduct < 0) {
                    float m1 = asteroids[i].radius;
                    float m2 = asteroids[j].radius;

                    float impulse = (2.0f * dotProduct) / (m1 + m2);

                    asteroids[i].velocity.x -= impulse * m2 * nx;
                    asteroids[i].velocity.y -= impulse * m2 * ny;
                    asteroids[j].velocity.x += impulse * m1 * nx;
                    asteroids[j].velocity.y += impulse * m1 * ny;
                }
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].position.x += asteroids[i].velocity.x;
            asteroids[i].position.y += asteroids[i].velocity.y;

            if (asteroids[i].position.x > screenWidth + asteroids[i].radius)
                asteroids[i].position.x = -asteroids[i].radius;
            else if (asteroids[i].position.x < -asteroids[i].radius)
                asteroids[i].position.x = screenWidth + asteroids[i].radius;

            if (asteroids[i].position.y > screenHeight + asteroids[i].radius)
                asteroids[i].position.y = -asteroids[i].radius;
            else if (asteroids[i].position.y < -asteroids[i].radius)
                asteroids[i].position.y = screenHeight + asteroids[i].radius;
        }
    }
}

void UpdateStarfield(Vector2 starfield[NUM_LAYERS][STARS_PER_LAYER])
{
    for (int i = 0; i < NUM_LAYERS; i++) {
        float speed = (i + 1) * 0.15f;

        for (int j = 0; j < STARS_PER_LAYER; j++) {
            starfield[i][j].y += speed;

            if (starfield[i][j].y > screenHeight) {
                starfield[i][j].y = 0;
                starfield[i][j].x = GetRandomValue(0, screenWidth);
            }
        }
    }
}

void CheckLevelClear(Asteroid* asteroids, int* level, Player* ship, Bullet** bulletsHead, Enemy* ufo, Boss* boss)
{
    if (boss->introTimer > 0.0f) {
        boss->introTimer -= GetFrameTime();

        if (boss->introTimer <= 0.0f) {
            boss->active = true;

            int spawnCount = 2;
            for (int i = 0; i < spawnCount; i++) {
                asteroids[i].active = true;
                asteroids[i].radius = 40.0f;

                for (int v = 0; v < ASTEROID_VERTICES; v++) {
                    asteroids[i].vertexOffsets[v] = GetRandomValue(80, 120) / 100.0f;
                }

                do {
                    asteroids[i].position.x = GetRandomValue(0, screenWidth);
                    asteroids[i].position.y = GetRandomValue(0, screenHeight);
                } while (CheckCollisionCircles(asteroids[i].position, 40.0f, ship->position, 150.0f));

                int speedBoost = (*level) * 20;
                asteroids[i].velocity.x = GetRandomValue(-200 - speedBoost, 200 + speedBoost) / 100.0f;
                asteroids[i].velocity.y = GetRandomValue(-200 - speedBoost, 200 + speedBoost) / 100.0f;
            }
        }
        return;
    }

    bool allDestroyed = true;
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            allDestroyed = false;
            break;
        }
    }

    if (boss->active)
        allDestroyed = false;

    if (allDestroyed) {
        (*level)++;

        Bullet* currentBullet = *bulletsHead;
        while (currentBullet != NULL) {
            Bullet* next = currentBullet->next;
            free(currentBullet);
            currentBullet = next;
        }
        *bulletsHead = NULL;

        if (*level % 5 == 0) {
            boss->introTimer = 3.0f;
            boss->active = false;

            boss->maxHealth = 15 + (*level);
            boss->health = boss->maxHealth;
            boss->radius = 50.0f;

            if (ship->position.x < screenWidth / 2.0f) {
                boss->position.x = screenWidth - 100;
            } else {
                boss->position.x = 100;
            }

            boss->position.y = screenHeight / 2;
            boss->velocity.x = 0;
            boss->velocity.y = 3.0f;
            boss->shootTimer = 1.0f;
            boss->bounceCount = 0;
            boss->isCrossing = false;

        } else {
            if (*level >= 3 && !ufo->active) {
                ufo->active = true;
                ufo->radius = 20.0f;
                ufo->position.x = 0;
                ufo->position.y = GetRandomValue(100, screenHeight - 100);
                ufo->velocity.x = (*level % 2 == 0) ? -2.0f : 2.0f;

                if (ufo->velocity.x < 0)
                    ufo->position.x = screenWidth;

                ufo->velocity.y = GetRandomValue(-100, 100) / 100.0f;
                ufo->shootTimer = 2.0f;
            }

            int spawnCount = 2 + (*level * 2);
            if (spawnCount > MAX_ASTEROIDS / 2)
                spawnCount = MAX_ASTEROIDS / 2;

            for (int i = 0; i < spawnCount; i++) {
                asteroids[i].active = true;
                asteroids[i].radius = 40.0f;

                for (int v = 0; v < ASTEROID_VERTICES; v++) {
                    asteroids[i].vertexOffsets[v] = GetRandomValue(80, 120) / 100.0f;
                }

                do {
                    asteroids[i].position.x = GetRandomValue(0, screenWidth);
                    asteroids[i].position.y = GetRandomValue(0, screenHeight);
                } while (CheckCollisionCircles(asteroids[i].position, 40.0f, ship->position, 150.0f));

                int speedBoost = (*level) * 20;
                asteroids[i].velocity.x = GetRandomValue(-200 - speedBoost, 200 + speedBoost) / 100.0f;
                asteroids[i].velocity.y = GetRandomValue(-200 - speedBoost, 200 + speedBoost) / 100.0f;
            }
        }
    }
}