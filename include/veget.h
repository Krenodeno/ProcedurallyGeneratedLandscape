#pragma once

#include "multiLayers.h"

#include <math.h>
#include <string>

namespace Veget{
    class Plant {
        protected:
            glm::vec2 _pos;
            int _age;
            float _sizeSeeding;
            bool _aLive = true;
        public:
            Plant(glm::vec2 pos, int age, float sizeSeeding): _pos(pos), _age(age), _sizeSeeding(sizeSeeding) {};

            glm::vec2 getPos() const { return _pos;};
            virtual float getSize() const = 0;
            virtual bool beDying() const = 0;
            virtual bool isSeeding() const = 0;
            virtual bool resolveConflit(Plant* autre) const = 0;
            virtual float strong() const = 0;
            bool isAlive() { return _aLive; }
            void kill() { _aLive = false;};
            int getAge() const {return _age;};
            void older(int i = 1) { _age += i; };
            float getSizeSeeding() const {return _sizeSeeding;};
    };
    class Sapin : public Plant
    {
        public:
            Sapin(glm::vec2 pos, int age): Plant(pos, age, 3) {};
            float getSize() const { return std::sqrt(_age)*.4;};
            bool beDying() const;
            bool isSeeding() const;
            bool resolveConflit(Plant* autre) const;
            float strong() const { return 50*(1/(10*sqrt(2*M_PI)))*exp(-(_age*_age)/200); }
    };

    class Case {
        private:
            std::vector<Plant*> _plants = {};
            Box2D _limits;
            Case* _neighbors[8] = {0};
        public:
            Case(Box2D b): _limits(b) {};

            bool inside(const glm::vec2& v) const 
                        {return _limits.inside(v);};
            int plantsSize() const 
                        { return _plants.size();};
            glm::vec2 plantPos(int i) const 
                        { return _plants[i]->getPos();};
            Box2D getLimits() const 
                        { return _limits; };
            std::vector<Plant*> getListPlants() const 
                        {return _plants;};
            Plant* intersectPlants(const glm::vec2 pos, const float sizePlant) const;

            void addPlant(Plant* plant) { _plants.push_back(plant); }
            Plant* getPlant(int i)  
                        { return _plants[i]; }
            void suprPlant(const int id) 
                        {_plants.erase(_plants.begin() + id);};
            void addNeighbor(const int id, Case *neighbor) 
                        { _neighbors[id] = neighbor;}; 
            Plant* intersectPlants(const bool initGrille, Plant* plant, const glm::vec2 max=glm::vec2(0,0));
            void copyPlants(const Case* autre, const glm::vec2 translate);

            Case* getNeighbor(int i) { return _neighbors[i]; };
    };

    void SimuleAll(MultiLayers& terrain);
    std::vector<Case*> create(MultiLayers& terrain);
    std::vector<Case*> initGrille(const int sizeGrille, const float sizeCase);
    int addPlantsGrille(std::vector<Case*>& grille, const glm::vec2 max);
    std::vector<Case*> extendGrille(std::vector<Case*>& grilleStart, const int sizeGrilleStart, const float sizeCase, const Box2D boxTerrain);
    bool terrainCondition(const glm::vec2 pos, MultiLayers& terrain);
    void addPlantsTerrain(std::vector<Case*>& grille, MultiLayers& terrain);
    void simuleYear(std::vector<Case*>& grille, MultiLayers& terrain);
    void savePosition(const std::vector<Case*>& grille, MultiLayers& terrain, std::string filename);
}