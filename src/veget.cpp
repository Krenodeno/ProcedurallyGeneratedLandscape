#include "veget.h"
#include "exporter.h"

#include <algorithm>
#include <glm/geometric.hpp>
#include <ctime>
#include <iostream> // @DEBUG
#include <fstream>
#include <string>

using namespace Veget;

bool Sapin::beDying() const {
    if(!_aLive) return true;
    if(_age > 50) return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) > .99;
    return false;
}

bool Sapin::isSeeding() const {
    if(_age > 25 && _age < 70) return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) > .9;
    return false;
}

bool Sapin::resolveConflit(Plant* autre) const {
    if(autre == nullptr || autre->isAlive())
        return false;
    if(strong() > autre->strong()) {
        autre->kill();
        return false;
    }
    return true;
}

Plant* Case::intersectPlants(const glm::vec2 pos, const float sizePlant) const {
    for(int i=0; i< _plants.size(); i++) {
        if( glm::distance(pos, plantPos(i)) < (sizePlant + _plants[i]->getSize()))
            return _plants[i];
    }
    return nullptr;
}

Plant* Case::intersectPlants(const bool initGrille, Plant* plant, const glm::vec2 max) {
    glm::vec2 pos = plant->getPos();
    float sizePlant = plant->getSize();
    Plant* r;

    r=intersectPlants(pos,sizePlant);
    if(r!=nullptr)
        return r;

    bool left = false;
    bool right = false;
    glm::vec2 maxX(max.x, 0.f);
    glm::vec2 maxY(0.f, max.y);

    if(pos.y - _limits.min.y < sizePlant && _neighbors[3] != 0) {
        left = true;
        r=_neighbors[3]->intersectPlants(pos,sizePlant);
        if(r!=nullptr)
            return r;

        if(initGrille) { // si la case est au bord de la grille
            r = _neighbors[3]->intersectPlants(pos+maxY,sizePlant);
            if(r!=nullptr)
                return r;
        }
    }

    if(_limits.max.y - pos.y < sizePlant && _neighbors[4] != 0) {
        right = true;
        r=_neighbors[4]->intersectPlants(pos,sizePlant);
        if(r!=nullptr)
            return r;

        if(initGrille) { // si la case est au bord de la grille
            r = _neighbors[4]->intersectPlants(pos-maxY,sizePlant);
            if(r!=nullptr)
                return r;
        }
    }

    if(pos.x - _limits.min.x < sizePlant && _neighbors[0] != 0) {
        if(_neighbors[1] != 0) {
            r=_neighbors[1]->intersectPlants(pos,sizePlant);
            if(r!=nullptr)
                return r;

            if(initGrille) { // si la case est au bord de la grille
                r = _neighbors[1]->intersectPlants(pos+maxX,sizePlant);
                if(r!=nullptr)
                    return r;
            }
        }

        if(left) {
            r=_neighbors[0]->intersectPlants(pos,sizePlant);
            if(r!=nullptr)
                return r;

            if(initGrille) { // si la case est au bord de la grille
                r = _neighbors[0]->intersectPlants(pos+max,sizePlant);
                if(r!=nullptr)
                    return r;
            }
        }

        if(right && _neighbors[2] != 0) {
            r=_neighbors[2]->intersectPlants(pos,sizePlant);
            if(r!=nullptr)
                return r;

            if(initGrille) { // si la case est au bord de la grille
                r = _neighbors[2]->intersectPlants(pos+maxX-maxY,sizePlant);
                if(r!=nullptr)
                    return r;
            }
        }
    }

    if(_limits.max.x - pos.x < sizePlant && _neighbors[6] != 0) {
        r=_neighbors[6]->intersectPlants(pos,sizePlant);
        if(r!=nullptr)
            return r;

        if(initGrille) { // si la case est au bord de la grille
            r = _neighbors[6]->intersectPlants(pos-maxX,sizePlant);
            if(r!=nullptr)
                return r;
        }

        if(left && _neighbors[5] != 0) {
            r=_neighbors[5]->intersectPlants(pos,sizePlant);
            if(r!=nullptr)
                return r;

            if(initGrille) { // si la case est au bord de la grille
                r = _neighbors[5]->intersectPlants(pos-maxX+maxY,sizePlant);
                if(r!=nullptr)
                    return r;
            }
        }


        if(right && _neighbors[7] != 0) {
            r=_neighbors[7]->intersectPlants(pos,sizePlant);
            if(r!=nullptr)
                return r;

            if(initGrille) { // si la case est au bord de la grille
                r = _neighbors[7]->intersectPlants(pos-max,sizePlant);
                if(r!=nullptr)
                    return r;
            }
        }
    }

    return nullptr;
}

void Case::copyPlants(const Case* autre,const glm::vec2 translate) {
    std::vector<Plant*> tmpList = autre->getListPlants();
    for(int i=0; i<tmpList.size(); i++) {
        _plants.push_back(new Sapin(tmpList[i]->getPos()+translate, tmpList[i]->getAge()));
    }
}

void Veget::SimuleAll(MultiLayers& terrain) {
    std::vector<Case*> grilleTerrain = create(terrain);
	
	RGBQUAD color;
	color.rgbRed = 0;
	color.rgbGreen = 255;
	color.rgbBlue = 0;
	color.rgbReserved = 255;
	float maxHeight = 0.1f;

	for (int c = 0; c < 10; c++) {
		int yearCount = 100;
		std::cout << "Simule " << c * yearCount << std::endl;
	    for(int i=0; i<yearCount; i++)
	        Veget::simuleYear(grilleTerrain, terrain);
	    std::cout << "export " << c * yearCount << std::endl;
		std::string texFilename(std::to_string(c*yearCount) + "veget.png");
	    //OBJExporter::save(terrain.heightfield() + terrain.veget(), "eroded_terrain_20.obj");
		TextureExporter::saveMask(terrain.veget(), maxHeight, color, texFilename);
	}

    savePosition(grilleTerrain, terrain, "treePositions.txt");
}

std::vector<Case*> Veget::create(MultiLayers& terrain) {
    float sizeCase = 20.f;
    int sizeGrilleBase = 10;

    std::vector<Case*> grille = initGrille(sizeGrilleBase, sizeCase);
    int nbPlants = addPlantsGrille(grille, glm::vec2(sizeCase*sizeGrilleBase,sizeCase*sizeGrilleBase));
    std::cout << nbPlants << " Plants" << std::endl;

    Box2D boxTerrain(terrain.veget().min, terrain.veget().max);
    std::vector<Case*> grilleTerrain = extendGrille(grille, sizeGrilleBase, sizeCase, boxTerrain);
    addPlantsTerrain(grilleTerrain, terrain);
    return grilleTerrain;
}

std::vector<Case*> Veget::initGrille(const int sizeGrille, const float sizeCase) {
    std::vector<Case*> grille;

    for(float i=0.f; i<sizeGrille; i+=1.f) {
        for(float j=0.f; j<sizeGrille; j+=1.f) {
            grille.push_back(new Case(Box2D(glm::vec2(sizeCase*i, sizeCase*j),glm::vec2(sizeCase*(i+1.f), sizeCase*(j+1.f)))));
        }
    }

    for(int i=0; i<sizeGrille; i++) {
        for(int j=0; j<sizeGrille; j++) {

            int tmpI = i-1;
            if(i==0) tmpI = sizeGrille-1;
            int tmpJ = j-1;
            if(j==0) tmpJ = sizeGrille-1;
            grille[i*sizeGrille+j]->addNeighbor(0,grille[tmpI*sizeGrille+tmpJ]);

            tmpI = i-1;
            if(i==0) tmpI = sizeGrille-1;
            grille[i*sizeGrille+j]->addNeighbor(1,grille[tmpI*sizeGrille+j]);

            tmpI = i-1;
            if(i==0) tmpI = sizeGrille-1;
            tmpJ = (j+1)%sizeGrille;
            grille[i*sizeGrille+j]->addNeighbor(2,grille[tmpI*sizeGrille+tmpJ]);

            tmpJ = j-1;
            if(j==0) tmpJ = sizeGrille-1;
            grille[i*sizeGrille+j]->addNeighbor(3,grille[i*sizeGrille+tmpJ]);

            tmpJ = (j+1)%sizeGrille;
            grille[i*sizeGrille+j]->addNeighbor(4,grille[i*sizeGrille+tmpJ]);

            tmpI = (i+1)%sizeGrille;
            tmpJ = j-1;
            if(j==0) tmpJ = sizeGrille-1;
            grille[i*sizeGrille+j]->addNeighbor(5,grille[tmpI*sizeGrille+tmpJ]);

            tmpI = (i+1)%sizeGrille;
            grille[i*sizeGrille+j]->addNeighbor(6,grille[tmpI*sizeGrille+j]);

            tmpI = (i+1)%sizeGrille;
            tmpJ = (j+1)%sizeGrille;
            grille[i*sizeGrille+j]->addNeighbor(7,grille[tmpI*sizeGrille+tmpJ]);
        }
    }

    return grille;
}

int Veget::addPlantsGrille(std::vector<Case*>& grille, const glm::vec2 max) {
    std::srand(std::time(nullptr));
    int nbPlants = 0;
    int nbTentative = 0;

    while(nbTentative < 1000000) {
        float x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/max.x));
        float y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/max.y));

        glm::vec2 pos(x,y);

        int id=0;
        while(!grille[id]->inside(pos))
            id++;

        int age = 25 + static_cast <int> (rand()) /( static_cast <int> (RAND_MAX/25)); // entre 25 et 50
        Sapin *tmp = new Sapin(pos, age);
        if(grille[id]->intersectPlants(true, tmp, max) == nullptr) {
            grille[id]->addPlant(tmp);
            nbPlants++;
            nbTentative = 0;
        } else {
            nbTentative++;
        }
    }

    return nbPlants;
}

std::vector<Case*> Veget::extendGrille(std::vector<Case*>& grilleStart, const int sizeGrilleStart, const float sizeCase, const Box2D boxTerrain) {
    glm::ivec2 sizeGrilleEnd( ceil((boxTerrain.max.x - boxTerrain.min.x)/sizeCase), ceil((boxTerrain.max.y - boxTerrain.min.y)/sizeCase));
    std::cout << sizeGrilleEnd.x << " - " << sizeGrilleEnd.y <<  std::endl;

    std::vector<Case*> grille;

    for(int y=0; y<sizeGrilleEnd.y; y++) {
        for(int x=0; x<sizeGrilleEnd.x; x++) {
            grille.push_back(new Case(Box2D(glm::vec2(x*sizeCase,y*sizeCase)+boxTerrain.min, glm::vec2((x+1)*sizeCase,(y+1)*sizeCase)+boxTerrain.min)));

            int tmpX = x%sizeGrilleStart;
            int tmpY = y%sizeGrilleStart;
            grille[grille.size()-1]->copyPlants( grilleStart[tmpY*sizeGrilleStart+tmpX], glm::vec2((x-tmpX)*sizeCase,(y-tmpY)*sizeCase)+boxTerrain.min);

            if(y>0) {
                grille[grille.size()-1]->addNeighbor(1,grille[(y-1)*sizeGrilleEnd.x+x]);
                grille[(y-1)*sizeGrilleEnd.x+x]->addNeighbor(6,grille[grille.size()-1]);

                if(x>0) {
                    grille[grille.size()-1]->addNeighbor(0,grille[(y-1)*sizeGrilleEnd.x+x-1]);
                    grille[(y-1)*sizeGrilleEnd.x+x-1]->addNeighbor(7,grille[grille.size()-1]);
                }

                if(x!=sizeGrilleEnd.x) {
                    grille[grille.size()-1]->addNeighbor(2,grille[(y-1)*sizeGrilleEnd.x+x+1]);
                    grille[(y-1)*sizeGrilleEnd.x+x+1]->addNeighbor(5,grille[grille.size()-1]);
                }
            }

            if(x>0) {
                grille[grille.size()-1]->addNeighbor(3,grille[y*sizeGrilleEnd.x+x-1]);
                grille[y*sizeGrilleEnd.x+x-1]->addNeighbor(4,grille[grille.size()-1]);
            }
        }
    }

    return grille;
}

bool Veget::terrainCondition(const glm::vec2 pos, MultiLayers& terrain) {
    if(terrain.veget().inside(pos)) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float sed = terrain.sediment().value(pos);
        if( r < std::min(glm::clamp((1.f - terrain.heightfield().gradient(pos)),0.f,1.f), glm::clamp((sed+sed)/10.f,0.f,1.f)))
            return true;
    }
    return false;
}

void Veget::addPlantsTerrain(std::vector<Case*>& grille, MultiLayers& terrain) {
    int nbPlants = 0;
    int total = 0;
    std::vector<std::pair<int,int> > ASuppr;

    for(int i=0; i< grille.size(); i++)
        for(int j=0; j<grille[i]->plantsSize(); j++) {
            glm::vec2 tmp = grille[i]->plantPos(j);
            if(terrainCondition(tmp, terrain)) {
                terrain.veget().setValue(tmp, grille[i]->getPlant(j)->getSize() * 5.f);
                nbPlants ++;
            } else {
                ASuppr.push_back(std::make_pair(i,j));
            }
            total++;
        }

    int compteur = 0;
    int prec = -1;
    for(int i=0; i<ASuppr.size(); i++) {
        std::pair<int,int> tmp = ASuppr[i];
        if(prec == tmp.first)
            compteur++;
        else
            compteur = 0;
        grille[tmp.first]->suprPlant(tmp.second-compteur);
        prec = tmp.first;
    }

    std::cout << nbPlants << " Plants sur " << total << std::endl;
}

void Veget::simuleYear(std::vector<Case*>& grille, MultiLayers& terrain) {
    std::vector<std::pair<int,int> > ASuppr;

    for(int i=0; i<grille.size(); i++)
        for(int j=0; j<grille[i]->plantsSize(); j++) {
            Plant* tmp = grille[i]->getPlant(j);
            tmp->older();
            Plant* autre = grille[i]->intersectPlants(false, tmp);
            if(tmp->beDying() || tmp->resolveConflit(autre) ) {
                terrain.veget().setValue(tmp->getPos(), 0.f);
                ASuppr.push_back(std::make_pair(i,j));
            } else {
                terrain.veget().setValue(tmp->getPos(), tmp->getSize() * 5.f);

                if(tmp->isSeeding()) {
                    float maxSize = tmp->getSizeSeeding();
                    float x = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxSize* 2)));
                    float y = static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxSize* 2)));

                    glm::vec2 pos = tmp->getPos() + glm::vec2(x-maxSize,y-maxSize);

                    if(terrainCondition(pos, terrain)) {
                        int id=0;
                        while(!grille[id]->inside(pos))
                            id++;

                        Sapin *graine = new Sapin(pos,1);
                        if(grille[id]->intersectPlants(false, graine) == nullptr) {
                            grille[id]->addPlant(graine);
                            terrain.veget().setValue(pos, 1.f);// taille en fonction de la fonction de la getSize
                        }
                    }
                }
            }
        }


    int compteur = 0;
    int prec = -1;
    for(int i=0; i<ASuppr.size(); i++) {
        std::pair<int,int> tmp = ASuppr[i];
        if(prec == tmp.first)
            compteur++;
        else
            compteur = 0;
        grille[tmp.first]->suprPlant(tmp.second-compteur);
        prec = tmp.first;
    }
}


void Veget::savePosition(const std::vector<Case*>& grille, MultiLayers& terrain, std::string filename) {
    Scal2D heightMap = terrain.heightfield();

    std::ofstream file;
    file.open(filename);

    if(!file.is_open())
        return;


    for(int i=0; i<grille.size(); i++)
        for(int j=0; j<grille[i]->plantsSize(); j++) {
            Plant* tmp = grille[i]->getPlant(j);
            file << tmp->getPos().x << " "
				 << tmp->getPos().y << " "
				 << heightMap.value(tmp->getPos()) << " "
				 << tmp->getSize() << "\n";
        }

    file.close();
}
