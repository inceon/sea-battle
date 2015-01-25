#include "game.h"
Game::Game(Player &_user, Player &_bot):user(&_user),bot(&_bot),player(0){
    user->genNew();
}

int Game::whoGoes(){
    return player;
}
bool Game::playerMove(){
    int b;
    if(!user->gameOver() && !bot->gameOver()){
        if(player==0){
            auto coord = user->shoot(); // pair<int, int>
            b = bot->checkHit(coord.first, coord.second); //  -1 - miss, 1 - hit, 2 - kill
            user->writeShoot(b);
            if(b==-1) player = 1;
        }else if(player==1){
            auto coord = bot->shoot(); // pair<int, int>
            b = user->checkHit(coord.first, coord.second); //  -1 - miss, 1 - hit, 2 - kill
            bot->writeShoot(b);
            if(b==-1) player = 0;
        }
        return true;
    }
    return false;
}
