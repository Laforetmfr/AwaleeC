#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <SDL_ttf.h>

#define TAILLE_PLATEAU 6
#define INITIAL_GRAINES 4
#define LARGEUR_CASE 150
#define HAUTEUR_CASE 150
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
        plateau[0][i] = INITIAL_GRAINES;
        plateau[1][i] = INITIAL_GRAINES;
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

void semer_graines(int joueur, int case_depart) {
    int graines = plateau[joueur][case_depart]; // Nombre de graines à semer
    plateau[joueur][case_depart] = 0;          // Vide la case de départ

    int i = case_depart;    // Position actuelle
    int camp = joueur;      // Camp actuel (0 ou 1)

    // Distribution des graines
    while (graines > 0) {
        if (camp == 0) { // Camp du bas (joueur 0)
            i++; // Avance vers la droite
            if (i >= TAILLE_PLATEAU) { // Si on dépasse la dernière case
                camp = 1; // Passe en haut (camp adverse)
                i = TAILLE_PLATEAU - 1; // Commence à droite
            }
        } else { // Camp du haut (joueur 1)
            i--; // Avance vers la gauche
            if (i < 0) { // Si on dépasse la première case
                camp = 0; // Retour en bas (camp joueur 0)
                i = 0; // Commence à gauche
            }
        }

        if (!(camp == joueur && i == case_depart)) { // Ne pas ressemer dans la case d'origine
            plateau[camp][i]++;
            graines--;
        }
    }

    // Capture des graines si on termine dans le camp adverse
    int adversaire = 1 - joueur;
    if (camp == adversaire) {
        while (i >= 0 && i < TAILLE_PLATEAU && (plateau[adversaire][i] == 2 || plateau[adversaire][i] == 3)) {
            score[joueur] += plateau[adversaire][i]; // Ajouter les graines au score
            plateau[adversaire][i] = 0;             // Vider la case capturée
            if (camp == 1) i--; // Avance vers la gauche si camp du haut
            else i++; // Avance vers la droite si camp du bas
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

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, score_text, textColor);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect = {10, 10, surfaceMessage->w, surfaceMessage->h};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

// Fonction pour afficher un message de fin de partie
void afficher_message_fin(SDL_Renderer* renderer, TTF_Font* font) {
    char message_fin[50];
    if (score[0] > score[1]) snprintf(message_fin, sizeof(message_fin), "Joueur 1 gagne avec %d points!", score[0]);
    else if (score[1] > score[0]) snprintf(message_fin, sizeof(message_fin), "Joueur 2 gagne avec %d points!", score[1]);
    else snprintf(message_fin, sizeof(message_fin), "Match nul!");

    SDL_Color textColor = { 255, 0, 0, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, message_fin, textColor);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect = {300, 300, surfaceMessage->w, surfaceMessage->h};
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

    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        return 1;
    }

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int case_choisie = get_case_from_click(e.button.x, e.button.y, joueur);
                if (case_choisie >= 0 && plateau[joueur][case_choisie] > 0) {
                    semer_graines(joueur, case_choisie);
                    if (fin_de_partie()) {
                        afficher_message_fin(renderer, font);  // Appel à la fonction afficher_message_fin
                        SDL_RenderPresent(renderer);  // Assure que le message est bien affiché
                        SDL_Delay(5000);  // Attendre 5 secondes avant de quitter
                        quit = 1;  // Met fin à la boucle après la fin de partie
                    }
                    joueur = 1 - joueur;  // Change de joueur
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        dessiner_plateau(renderer);
        afficher_scores(renderer, font);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}

