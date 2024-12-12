#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_main.h>
#include <stdio.h>
#include <windows.h>

#define TAILLE_PLATEAU 6
#define INITIAL_GRANDES 4
#define LARGEUR_CASE 100
#define HAUTEUR_CASE 100
#define ESPACE 10

int plateau[2][TAILLE_PLATEAU];

// Fonction pour initialiser le plateau
void initialiser_jeu() {
    for (int i = 0; i < TAILLE_PLATEAU; i++) {
        plateau[0][i] = INITIAL_GRANDES;
        plateau[1][i] = INITIAL_GRANDES;
    }
}

// Fonction pour dessiner le plateau
void dessiner_plateau(SDL_Renderer* renderer) {
    for (int i = 0; i < TAILLE_PLATEAU; i++) {
        SDL_Rect rect1 = { i * (LARGEUR_CASE + ESPACE), 400, LARGEUR_CASE, HAUTEUR_CASE };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect1);

        for (int j = 0; j < plateau[0][i]; j++) {
            SDL_Rect graine1 = { rect1.x + (LARGEUR_CASE / 2) - 10 + (j % 2) * 20, rect1.y + (HAUTEUR_CASE / 2) - 10 + (j / 2) * 20, 10, 10 };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &graine1);
        }

        SDL_Rect rect2 = { i * (LARGEUR_CASE + ESPACE), 50, LARGEUR_CASE, HAUTEUR_CASE };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect2);

        for (int j = 0; j < plateau[1][i]; j++) {
            SDL_Rect graine2 = { rect2.x + (LARGEUR_CASE / 2) - 10 + (j % 2) * 20, rect2.y + (HAUTEUR_CASE / 2) - 10 + (j / 2) * 20, 10, 10 };
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &graine2);
        }
    }
}

// Fonction pour obtenir la case à partir d'un clic
int get_case_from_click(int x, int y) {
    if (y > 50 && y < 150) {
        for (int i = 0; i < TAILLE_PLATEAU; i++) {
            if (x >= i * (LARGEUR_CASE + ESPACE) && x <= i * (LARGEUR_CASE + ESPACE) + LARGEUR_CASE) {
                return i;
            }
        }
    } else if (y > 400 && y < 500) {
        for (int i = 0; i < TAILLE_PLATEAU; i++) {
            if (x >= i * (LARGEUR_CASE + ESPACE) && x <= i * (LARGEUR_CASE + ESPACE) + LARGEUR_CASE) {
                return i;
            }
        }
    }
    return -1;
}

// Fonction principale
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Jeu de l'Awalé", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    initialiser_jeu();

    int quit = 0;
    SDL_Event e;
    int joueur = 0;
    int case_choisie = -1;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                case_choisie = get_case_from_click(e.button.x, e.button.y);
                if (case_choisie >= 0 && plateau[joueur][case_choisie] > 0) {
                    int graines = plateau[joueur][case_choisie];
                    plateau[joueur][case_choisie] = 0;
                    for (int i = case_choisie + 1; graines > 0; i++) {
                        if (i == TAILLE_PLATEAU) i = 0;
                        plateau[joueur == 0 ? 1 : 0][i]++;
                        graines--;
                    }
                    joueur = 1 - joueur;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        dessiner_plateau(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
