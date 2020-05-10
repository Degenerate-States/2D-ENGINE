#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <complex>
#include "assets.h"
#include "components.h"
#include "engine.h"
#include "gameManager.h"
#include "config.h"

using namespace std;

int main(int argc, char **argv) {
    Assets* assets = new Assets();
    Config* cfg = new Config();
    Stats* stats = new Stats();
    assets->init();
    cfg->init();
    stats->init();

    GameManager gameMng;
    gameMng.init(cfg,assets,stats);

    free(assets);
    free(cfg);
    free(stats);

    gameMng.gameLoop();
    gameMng.clean();
    return 0;
}