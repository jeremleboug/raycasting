#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define MAP_WIDTH 18
#define MAP_HEIGHT 8
#define M_PI 3.14159265358979323846

int screenWidth = 640;
int screenHeight = 480;



int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre SDL
    SDL_Window* window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Création du rendu (renderer) SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }
    if (Mix_Init(MIX_INIT_OGG) < 0) {
    printf("Mix_Init failed: %s\n", Mix_GetError());
    return 1;
}
Mix_Music* dumbswaytodie = Mix_LoadMUS("dumb ways to die.mp4");

    // Position du joueur
    float playerX = 4.5;
    float playerY = 4.5;
    float playerAngle = 0;

    // Projection de la caméra
    float fov = M_PI / 3.0; // Champ de vision (60 degrés)

    // Boucle principale
    int quit = 0;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        // Déplacer vers l'avant
                        playerX += cos(playerAngle) * 0.1;
                        playerY += sin(playerAngle) * 0.1;
                        break;
                    case SDLK_DOWN:
                        // Déplacer vers l'arrière
                        playerX -= cos(playerAngle) * 0.1;
                        playerY -= sin(playerAngle) * 0.1;
                        break;
                    case SDLK_LEFT:
                        // Tourner à gauche
                        playerAngle -= 0.1;
                        break;
                    case SDLK_RIGHT:
                        // Tourner à droite
                        playerAngle += 0.1;
                        break;
                }
            }
        }

        // Effacer l'écran (rendre tout l'écran blanc)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        // Déclaration des couleurs pour le mur et le sol
SDL_Color wallColor = {87, 87, 87, 255}; // gris pour les murs
SDL_Color floorColor = {98, 44, 0, 255}; // marron pour le sol
SDL_Color skyColor = {0, 0, 255, 255}; // Bleu pour le ciel

// Boucle pour chaque colonne d'écran
for (int x = 0; x < screenWidth; x++) {
    // Angle du rayon
    float rayAngle = (playerAngle - fov / 2.0) + ((float)x / (float)screenWidth) * fov;

    // Coordonnées du rayon
    float rayX = playerX;
    float rayY = playerY;

    float stepSize = 0.01; // Taille de l'étape du rayon

    int hitWall = 0;
    while (!hitWall) {
        rayX += cos(rayAngle) * stepSize;
        rayY += sin(rayAngle) * stepSize;

        // Vérifier si le rayon a frappé un mur
        int mapX = (int)rayX;
        int mapY = (int)rayY;
        if (worldMap[mapX][mapY] == 1) {
            hitWall = 1;
        }
    }

    // Calculer la distance au mur
 float distance = sqrt((rayX - playerX) * (rayX - playerX) + (rayY - playerY) * (rayY - playerY));
distance = distance * cos(rayAngle - playerAngle);

    float brightness = 1.0 / (distance * 0.1);
    Uint8 b = (Uint8)(wallColor.b * brightness);
    // Calculer la hauteur de la ligne à dessiner
    int lineHeight = (int)(screenHeight / distance);

    // Dessiner la ligne verticale
    for (int y = 0; y < screenHeight; y++) {
        if (y < (screenHeight - lineHeight) / 2) {
            // Dessiner le ciel en haut
            SDL_SetRenderDrawColor(renderer, skyColor.r, skyColor.g, skyColor.b, skyColor.a); // Bleu
        } else if (y >= (screenHeight - lineHeight) / 2 && y < (screenHeight + lineHeight) / 2) {
SDL_SetRenderDrawColor(renderer, wallColor.r-(distance/0.2), wallColor.g-(distance/0.2), wallColor.b-(distance/0.2), wallColor.a); // Bleu
        } else {
            // Dessiner le sol en bas
            SDL_SetRenderDrawColor(renderer, floorColor.r, floorColor.g, floorColor.b, floorColor.a);
        }

        // Dessiner un pixel à la position actuelle (x, y)
        SDL_RenderDrawPoint(renderer, x, y);
    }
    printf("X=%f Y=%f \n", playerX, playerY);
        if((playerX<=2) && (playerY<=2)){
            printf("dumbs way to die");
            //lancement de la video
            Mix_PlayMusic(dumbswaytodie, 1);
        }
    }

        // Rafraîchir l'affichage
        SDL_RenderPresent(renderer);

    }

    // Libérer les ressources SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
//gcc -o bin/jeu src/main.c -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
