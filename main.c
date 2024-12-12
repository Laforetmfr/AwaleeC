#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <SDL_ttf.h>

#define TAILLE_PLATEAU 6
#define INITIAL_GRANDES 4
#define LARGEUR_CASE 130
#define HAUTEUR_CASE 130
#define ESPACE 10

int plateau[2][TAILLE_PLATEAU];
int score[2] = {0, 0}; // Scores des joueurs

// Prototypes des fonctions
void initialiser_jeu();
void dessiner_plateau(SDL_Renderer* renderer);
int get_case_from_click(int x, int y, int joueur);
void semer_graines(int joueur, int case_depart);
int fin_de_partie();
void afficher_scores(SDL_Renderer* renderer, TTF_Font* font);

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
        // Cases du joueur 1
        SDL_Rect rect1 = { i * (LARGEUR_CASE + ESPACE), 400, LARGEUR_CASE, HAUTEUR_CASE };
        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255); // Marron clair
        SDL_RenderFillRect(renderer, &rect1);

        for (int j = 0; j < plateau[0][i]; j++) {
            SDL_Rect graine1 = { rect1.x + (LARGEUR_CASE / 2) - 10 + (j % 2) * 20,
                                 rect1.y + (HAUTEUR_CASE / 2) - 10 + (j / 2) * 20,
                                 10, 10 };
            SDL_SetRenderDrawColor(renderer, 255, 255, 153, 255); // Jaune clair
            SDL_RenderFillRect(renderer, &graine1);
        }

        // Cases du joueur 2
        SDL_Rect rect2 = { i * (LARGEUR_CASE + ESPACE), 50, LARGEUR_CASE, HAUTEUR_CASE };
        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255); // Marron clair
        SDL_RenderFillRect(renderer, &rect2);

        for (int j = 0; j < plateau[1][i]; j++) {
            SDL_Rect graine2 = { rect2.x + (LARGEUR_CASE / 2) - 10 + (j % 2) * 20,
                                 rect2.y + (HAUTEUR_CASE / 2) - 10 + (j / 2) * 20,
                                 10, 10 };
            SDL_SetRenderDrawColor(renderer, 255, 255, 153, 255); // Jaune clair
            SDL_RenderFillRect(renderer, &graine2);
        }
    }
}

// Fonction pour obtenir la case à partir d'un clic
int get_case_from_click(int x, int y, int joueur) {
    if ((joueur == 0 && y >= 400 && y < 500) || (joueur == 1 && y >= 50 && y < 150)) {
        for (int i = 0; i < TAILLE_PLATEAU; i++) {
            if (x >= i * (LARGEUR_CASE + ESPACE) && x <= i * (LARGEUR_CASE + ESPACE) + LARGEUR_CASE) {
                return i;
            }
        }
    }
    return -1;
}

// Fonction pour semer les graines
void semer_graines(int joueur, int case_depart) {
    int graines = plateau[joueur][case_depart];
    plateau[joueur][case_depart] = 0;
    int i = case_depart + 1;
    int adversaire = 1 - joueur;

    while (graines > 0) {
        if (i >= TAILLE_PLATEAU) {
            joueur = adversaire;
            i = 0;
        }
        if (!(joueur == adversaire && i == case_depart)) { // On saute la case d'origine
            plateau[joueur][i]++;
            graines--;
        }
        i++;
    }

    // Vérifie les captures
    i--;
    if (joueur != adversaire) { // Captures seulement dans le camp adverse
        while (i >= 0 && plateau[adversaire][i] >= 2 && plateau[adversaire][i] <= 3) {
            score[1 - adversaire] += plateau[adversaire][i];
            plateau[adversaire][i] = 0;
            i--;
        }
    }
}

// Fonction pour détecter la fin de la partie
int fin_de_partie() {
    int joueur1_vide = 1, joueur2_vide = 1;
    for (int j = 0; j < TAILLE_PLATEAU; j++) {
        if (plateau[0][j] > 0) joueur1_vide = 0;
        if (plateau[1][j] > 0) joueur2_vide = 0;
    }
    return joueur1_vide || joueur2_vide;
}

// Fonction pour afficher les scores
void afficher_scores(SDL_Renderer* renderer, TTF_Font* font) {
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Joueur 1: %d - Joueur 2: %d", score[0], score[1]);
    // Affichage du texte (nécessite SDL_ttf)
    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, score_text, textColor);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; // Création d'un rectangle pour afficher le texte
    Message_rect.x = 10;
    Message_rect.y = 10;
    Message_rect.w = surfaceMessage->w;
    Message_rect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

// Fonction principale
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Jeu de l'Awalé", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 700, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    initialiser_jeu();
    int quit = 0, joueur = 0;

    // Charger la police
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (font == NULL) {
        printf("Erreur lors du chargement de la police : %s\n", TTF_GetError());
        return 1;  // Arrêt du programme en cas d'erreur
    }

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int case_choisie = get_case_from_click(e.button.x, e.button.y, joueur);
                if (case_choisie >= 0 && plateau[joueur][case_choisie] > 0) {
                    semer_graines(joueur, case_choisie);
                    if (fin_de_partie()) quit = 1;
                    joueur = 1 - joueur;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        dessiner_plateau(renderer);
        afficher_scores(renderer, font); // Afficher les scores avec la police chargée
        SDL_RenderPresent(renderer);
    }

    // Libérer les ressources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
