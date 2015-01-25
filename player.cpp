 #include "player.h"
bool Player::Freedom(const int x, const int y, int area[10][10]){
    int d[8][2] = {{0,1},{1,0},{0,-1},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};
    if((x >= 0)&&(x < 10)&&(y >= 0)&&(y < 10)&&(area[x][y] == 0)){
        for(int i=0;i<8;i++){
            int dx = x + d[i][0];
            int dy = y + d[i][1];
            if((dx >= 0)&&(dx < 10)&&(dy >= 0)&&(dy < 10)&&(area[dx][dy] > 0)) return false;
        }
        return true;
    }else return false;
}
Player::Player():X(0),Y(0),lastMove(false), ships(0){
    counter = 0;
    srand(static_cast<unsigned>(time(NULL)));
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
			shootArea[i][j] = myArea[i][j] = woundArea[i][j] = 0;
    bool b;
    for(int n=3;n>=0;n--){
        for(int m=0;m<=3-n;m++){
            do{
                int x = rand() % 10;
                int y = rand() % 10;
                int kx = rand() % 2; //0 - left, right; 1 - top, bottom
                int ky = (kx==0)?1:0;
                b = true;
                for(int i=0;i<=n;i++) if(!Freedom(x + kx * i, y + ky * i, myArea)) b = false;
                if(b) for(int i=0;i<=n;i++) myArea[x+kx*i][y+ky*i] = 1;
            }while(!b);
        }
    }
}
Player::~Player(){

}
bool Player::checkCell(int x, int y){
    return (myArea[x][y]==1);
}
int Player::getShootCell(int x, int y){
    return shootArea[x][y];
}
int Player::getWoundCell(int x, int y){
    return woundArea[x][y];
}
bool Player::gameOver(){
    if(ships==10) return true;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(shootArea[i][j]==0) return false;
    return true;
}
void Player::genNew(){
    srand(static_cast<unsigned>(time(NULL)));
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
			shootArea[i][j] = myArea[i][j] = 0;

    bool b;
    for(int n=3;n>=0;n--){
        for(int m=0;m<=3-n;m++){
            do{
                int x = rand() % 10;
                int y = rand() % 10;
                int kx = rand() % 2; //0 - left, right; 1 - top, bottom
                int ky = (kx==0)?1:0;
                b = true;
                for(int i=0;i<=n;i++) if(!Freedom(x + kx * i, y + ky * i, myArea)) b = false;
                if(b) for(int i=0;i<=n;i++) myArea[x+kx*i][y+ky*i] = 1;
            }while(!b);
        }
    }
}
std::pair<int, int> Player::shoot(){
    srand(static_cast<unsigned>(time(NULL)));
    counter++;
    if(lastMove){
        if(prevX==-1 && prevY==-1){
            int pos[4] = {0, 0, 0, 0}; // x-left, x-right, y-top, y-bottom
            if(shootArea[X-1][Y]==1) pos[0]++;
            if(shootArea[X+1][Y]==1) pos[1]++;
            if(shootArea[X][Y-1]==1) pos[2]++;
            if(shootArea[X][Y+1]==1) pos[3]++;

            prevX = X; prevY = Y;

            if(pos[0]==0 && X-1>=0 && X--) return std::make_pair(X, Y);
            if(pos[1]==0 && X+1<10 && X++) return std::make_pair(X, Y);
            if(pos[2]==0 && Y-1>=0 && Y--) return std::make_pair(X, Y);
            if(pos[3]==0 && Y+1<10 && Y++) return std::make_pair(X, Y);
            do{
                X = rand() % 10;
                Y = rand() % 10;
            }while(shootArea[X][Y]!=0);
            return std::make_pair(X, Y);
        }else{
            if(prevX==X){
                if(prevY<Y){
                    if(Y+1>=10){
                        Y = prevY-1;
                        if(shootArea[X][Y]==-1){
                            for(int &i=Y;i<10;i++) if(shootArea[X][Y]==0) break;
                            prevY = Y-1;
                        }
                        return std::make_pair(X, Y);
                    }else{
                        prevY = Y;
                        Y++;
                        if(shootArea[X][Y]==-1){
                            for(int &i=Y;i>=0;i--) if(shootArea[X][Y]==0) break;
                            prevY = Y+1;
                        }
                        return std::make_pair(X, Y);
                    }
                }else{
                    if(Y-1<0){
                        Y = prevY+1;
                        if(shootArea[X][Y]==-1){
                            for(int &i=Y;i>=0;i--) if(shootArea[X][Y]==0) break;
                            prevY = Y-1;
                        }
                        return std::make_pair(X, Y);
                    }else{
                        prevY = Y;
                        Y--;
                        if(shootArea[X][Y]==-1){
                            for(int &i=Y;i>=0;i++) if(shootArea[X][Y]==0) break;
                            prevY = Y+1;
                        }
                        return std::make_pair(X, Y);
                    }
                }
            }else{
                if(prevX<X){
                    if(X+1>=10){
                        X = prevX-1;
                        if(shootArea[X][Y]==-1){
                            for(int &i=X;i<10;i++) if(shootArea[X][Y]==0) break;
                            prevX = X-1;
                        }
                        return std::make_pair(X, Y);
                    }else{
                        prevX = X;
                        X++;
                        if(shootArea[X][Y]==-1){
                            for(int &i=X;i>=0;i--) if(shootArea[X][Y]==0) break;
                            prevX = X-1;
                        }
                        return std::make_pair(X, Y);
                    }
                }else{
                    if(X-1<0){
                        X = prevX+1;
                        if(shootArea[X][Y]==-1){
                            for(int &i=X;i>=0;i--) if(shootArea[X][Y]==0) break;
                            prevX = X-1;
                        }
                        return std::make_pair(X, Y);
                    }else{
                        prevX = X;
                        X--;
                        if(shootArea[X][Y]==-1){
                            for(int &i=X;i<10;i++) if(shootArea[X][Y]==0) break;
                            prevX = X-1;
                        }
                        return std::make_pair(X, Y);
                    }
                }
            }
        }
    }else{
        prevX = -1; prevY = -1;
        do{
            X = rand() % 10;
            Y = rand() % 10;
        }while(shootArea[X][Y]!=0);
        return std::make_pair(X, Y);
    }
}
int Player::checkHit(int x, int y){
    /* returns  0 - miss, 1 - hit, 2 - kill */
    if(myArea[x][y]==1){
        woundArea[x][y] = 1;
        int xkl = (checkCell(x+1,y) || checkCell(x-1,y))?1:0,
            ykl = 1 - xkl,
            _x(x),
            _y(y),
            k(0), p(0);
        for(;checkCell(_x, _y);_x += xkl, _y += ykl){
            k++;
            if(woundArea[_x][_y]==1) p++;
        }
        for(_x=x,_y=y; checkCell(_x, _y); _x -= xkl, _y -= ykl){
            k++;
            if(woundArea[_x][_y]==1) p++;
        }
        if(k==p){
            int xkl = (woundArea[x+1][y]==1 || woundArea[x-1][y]==1?1:0),
                ykl = 1 - xkl,
                _x(x),
                _y(y),
                dx, dy,
                d[8][2] = {{0,1},{1,0},{0,-1},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};
            for(;woundArea[_x][_y]==1;_x += xkl, _y += ykl)
                for(int i=0;i<8;i++){
                    dx = _x + d[i][0];
                    dy = _y + d[i][1];
                    if((dx >= 0)&&(dx < 10)&&(dy >= 0)&&(dy < 10))
                        if(woundArea[dx][dy]!=1) woundArea[dx][dy] = -1;
                }

            for(_x = x,_y = y; woundArea[_x][_y]==1; _x -= xkl, _y -= ykl)
                for(int i=0;i<8;i++){
                    dx = _x + d[i][0];
                    dy = _y + d[i][1];
                    if((dx >= 0)&&(dx < 10)&&(dy >= 0)&&(dy < 10))
                        if(woundArea[dx][dy]!=1) woundArea[dx][dy] = -1;
                }
            ships++;
            return 2;
        }else{
            return 1;
        }
    }else{
        woundArea[x][y] = -1;
        return -1;
    }
}
void Player::writeShoot(int b){
    /* -1 - miss, 1 - hit, 2 - kill */
    if(b==1){
        shootArea[X][Y] = 1;
        lastMove = true;
    }else if(b==2){
        shootArea[X][Y] = 1;
        int xkl = (shootArea[X+1][Y]==1 || shootArea[X-1][Y]==1?1:0),
            ykl = 1 - xkl,
            _x(X),
            _y(Y),
            dx, dy,
            d[8][2] = {{0,1},{1,0},{0,-1},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};
        for(;shootArea[_x][_y]==1;_x += xkl, _y += ykl)
            for(int i=0;i<8;i++){
                dx = _x + d[i][0];
                dy = _y + d[i][1];
                if((dx >= 0)&&(dx < 10)&&(dy >= 0)&&(dy < 10))
                    if(shootArea[dx][dy]!=1) shootArea[dx][dy] = -1;
            }

        for(_x=X,_y=Y; shootArea[_x][_y]==1; _x -= xkl, _y -= ykl)
            for(int i=0;i<8;i++){
                dx = _x + d[i][0];
                dy = _y + d[i][1];
                if((dx >= 0)&&(dx < 10)&&(dy >= 0)&&(dy < 10))
                    if(shootArea[dx][dy]!=1) shootArea[dx][dy] = -1;
            }
        lastMove = false;
    }else{
        shootArea[X][Y] = -1;
        lastMove = false;
    }
}
