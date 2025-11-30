#include "Core/Entity.h" 
#include <cmath> 
#include <iostream> 
#include <algorithm> 
namespace Ecosystem { 
namespace Core { 
// 🏗 CONSTRUCTEUR PRINCIPAL 
Entity::Entity(EntityType type, Vector2D pos, std::string entityName) 
    : mType(type), position(pos), name(entityName),  
      mRandomGenerator(std::random_device{}())  // Initialisation du générateur alé
 { 
    //INITIALISATION SELON LE TYPE 
    switch(mType) { 
        case EntityType::HERBIVORE: 
            mEnergy = 80.0f; 
            mMaxEnergy = 150.0f; 
            mMaxAge = 200; 
            color = Color::Blue(); 
            size = 8.0f; 
            break; 
        case EntityType::CARNIVORE: 
            mEnergy = 100.0f; 
            mMaxEnergy = 200.0f; 
            mMaxAge = 150; 
            color = Color::Red(); 
            size = 12.0f; 
            break; 
        case EntityType::PLANT: 
            mEnergy = 50.0f; 
            mMaxEnergy = 100.0f; 
            mMaxAge = 300; 
            color = Color::Green(); 
            size = 6.0f; 
            break; 
    }
    mAge = 0; 
    mIsAlive = true; 
    mVelocity = GenerateRandomDirection(); 
    std::cout << "🌱 Entité créée: " << name << " à (" << position.x << ", " << position.y << ")";
 } 

// 🏗 CONSTRUCTEUR DE COPIE 
Entity::Entity(const Entity& other) 
    : mType(other.mType), position(other.position), name(other.name + "_copy"), 
      mEnergy(other.mEnergy * 0.7f),  // Enfant a moins d'énergie 
      mMaxEnergy(other.mMaxEnergy), 
      mAge(0),  // Nouvelle entité, âge remis à 0 
      mMaxAge(other.mMaxAge), 
      mIsAlive(true), 
      mVelocity(other.mVelocity), 
      color(other.color), 
      size(other.size * 0.8f),  // Enfant plus petit 
      mRandomGenerator(std::random_device{}()) 
{ 
 
    std::cout << " 👶 Copie d'entité créée: " << name << std::endl; 
}
// 🗑 DESTRUCTEUR 
Entity::~Entity() { 
    std::cout << "💀Entité détruite: " << name << " (Âge: " << mAge << ")" << std::endl;
 } 

 //MISE À JOUR PRINCIPALE 
void Entity::Update(float deltaTime,const std::vector<Food>& foodSources) { 
    if (!mIsAlive) return;
    Vector2D f =SeekFood(foodSources); 
    
    //PROCESSUS DE VIE 
    ConsumeEnergy(deltaTime); 
    Age(deltaTime); 
    Move(deltaTime, foodSources); 
    CheckVitality(); 
} 
//MOUVEMENT 
void Entity::Move(float deltaTime,const std::vector<Food>& foodSources) { 
    
    if (mType == EntityType::PLANT) return;  // Les plantes ne bougent pas
    if(mEnergy<70.0f){//permet de verifier si l'energie de l'entité est inferieur a l'energie minimale
        position = position + SeekFood(foodSources)* deltaTime* 0.025f;

    } 
    
    //Comportement aléatoire occasionnel 
    std::uniform_real_distribution<float> chance(0.0f, 1.0f); 
    if (chance(mRandomGenerator) < 0.02f) { 
        mVelocity = GenerateRandomDirection(); 
    }
    position= StayInBounds(1200.0f, 600.0f);//cadre dans lequel les entites doivent se deplacer
    //Application du mouvement 
    position = position + mVelocity * deltaTime * 20.0f;
    
    Vector2D force{0.0f,0.0f};//force represente la direction de l'intensité du mouvement que l'entité doit utiliser pour se déplacer
    this->ApplyForce(force);
    // Consommation d'énergie due au mouvement 
    mEnergy -= mVelocity.Distance(Vector2D(0, 0)) * deltaTime * 0.1f;
    
    
} 
// 🍽 MANGER
 void Entity::Eat(float energy) { 
    mEnergy += energy; 
    if (mEnergy > mMaxEnergy) { 
        mEnergy = mMaxEnergy; 
    } 
        std::cout << "🍽 " << name << " mange et gagne " << energy << " énergie" << std::endl;
    }
 
    //CONSOMMATION D'ÉNERGIE 
void Entity::ConsumeEnergy(float deltaTime) { 
    float baseConsumption = 0.0f; 
    switch(mType) { 
        case EntityType::HERBIVORE: 
            baseConsumption = 1.5f; 
            break; 
        case EntityType::CARNIVORE: 
            baseConsumption = 2.0f; 
            break; 
        case EntityType::PLANT: 
            baseConsumption = -0.5f;  // Les plantes génèrent de l'énergie ! 
            break; 
    }
    mEnergy -= baseConsumption * deltaTime; 
} 

// VIEILLISSEMENT 
void Entity::Age(float deltaTime) { 
    mAge += static_cast<int>(deltaTime * 10.0f);  // Accéléré pour la simulation 
} 

// ❤VÉRIFICATION DE LA SANTÉ 
void Entity::CheckVitality() { 
    if (mEnergy <= 0.0f || mAge >= mMaxAge) { 
        mIsAlive = false; 
        std::cout << "💀 " << name << " meurt - "; 
        if (mEnergy <= 0) std::cout << "Faim"; 
        else std::cout << "Vieillesse"; 
        std::cout << std::endl; 
    }
 } 

 // REPRODUCTION 
bool Entity::CanReproduce() const { 
    return mIsAlive && mEnergy > mMaxEnergy * 0.8f && mAge > 20; 
} 
std::unique_ptr<Entity> Entity::Reproduce() { 
    if (!CanReproduce()) return nullptr; 
    
    // possibilite de reproduction 
    std::uniform_real_distribution<float> chance(0.0f, 1.0f); 
    if (chance(mRandomGenerator) < 0.3f) { 
    }
        mEnergy *= 0.6f;  // valeur énergétique de la reproduction 
        return std::make_unique<Entity>(*this);  // Utilisation du constructeur de copi
    return nullptr; 
} 

// GÉNÉRATION DE DIRECTION ALÉATOIRE 
Vector2D Entity::GenerateRandomDirection() { 
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f); 
    return Vector2D(dist(mRandomGenerator), dist(mRandomGenerator)); 
} 

// CALCUL DE LA COULEUR BASÉE SUR L'ÉTAT 
Color Entity::CalculateColorBasedOnState() const { 
    float energyRatio = GetEnergyPercentage(); 
    Color baseColor = color; 
    
    // Rouge si faible énergie 
    if (energyRatio < 0.3f) { 
        baseColor.r = 255; 
        baseColor.g = static_cast<uint8_t>(baseColor.g * energyRatio); 
        baseColor.b = static_cast<uint8_t>(baseColor.b * energyRatio); 
    }
    return baseColor; 
} 
 Vector2D Entity::SeekFood(const std::vector<Food>& foodSources) const {
   std::vector<Vector2D>searchFood;
     Vector2D desireDirection{0.0f , 0.0f};
   float minDist = std::numeric_limits<float>::max();
   //boucle qui parcours la liste de nourriture
   for(const auto& food :foodSources ){
    float Dist = position.Distance(food.position);
    if(Dist < minDist ){
        minDist = Dist;
        desireDirection = Vector2D(food.position.x-position.x,food.position.y-position.y); 
    
    }
    
   }
   //normalisation de la direction 
   float lenght = std::sqrt(desireDirection.x*desireDirection.x+desireDirection.y*desireDirection.y);
    if(lenght>0.0f){
    desireDirection.x/=lenght;
    desireDirection.y/=lenght;
    }
   return desireDirection; //permet de retourner la position de la nourriture trouvee
 }
 
    Vector2D Entity::AvoidPredators(const std::vector<Entity>& predators) const {
float minDist = std::numeric_limits<float>::max();
const Entity* close=nullptr;
Vector2D f={0,0};
for(const auto& pre:predators){
    if(pre.mType != EntityType::CARNIVORE)
    continue;
    float dist = position.Distance(pre.position);
    if(dist < minDist ){
    minDist = dist;                             
    close = &pre;
   }else if(!close) {
    return {0,0};//si l'on a aucun predateur on retourne le vecteur null
   }
   f.x=position.x-close->position.x;
   f.y=position.y-close->position.y;
   } 
    return f;
}
Vector2D Entity::StayInBounds (float worldWidth, float worldHeight) const{
   float w=worldWidth, h=worldHeight;
   Vector2D p=position;
   
 if (p.x<0){
   p.x=6.03f; 
}
 if(p.x>w-6.02f){
  p.x=w-6.02f;
}
 if(p.y<0){
  p.y=6.02f;
}
 if(p.y>h){
  p.y=h-6.0f;
}
return p;
}
void Entity::ApplyForce(Vector2D force){
    mVelocity = mVelocity + force;
     
}

// RENDU GRAPHIQUE 
void Entity::Render(SDL_Renderer* renderer) const { 
    if (!mIsAlive) return; 
    Color renderColor = CalculateColorBasedOnState(); 
    SDL_FRect rect = { 
        position.x - size / 2.0f, 
        position.y - size / 2.0f, 
        size, 
        size 
    }; 
    SDL_SetRenderDrawColor(renderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
    SDL_RenderFillRect(renderer, &rect); 
    
    // Indicateur d'énergie (barre de vie) 
    if (mType != EntityType::PLANT) { 
        float energyBarWidth = size * GetEnergyPercentage(); 
        SDL_FRect energyBar = { 
            position.x - size / 2.0f, 
            position.y - size / 2.0f - 3.0f, 
            energyBarWidth, 
            2.0f 
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_RenderFillRect(renderer, &energyBar); 
    }
 } 
} // namespace Core 
} // namespace Ecosystem