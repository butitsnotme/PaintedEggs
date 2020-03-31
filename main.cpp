#ifndef NDEBUG
#define _DEBUG
#endif

#define PI 3.14159

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <list>
#include <cstdint>
#include <tuple>
#include <memory>
#include <cstdio>
#include <sstream>
#include <iomanip>

template<typename T>
T clamp(T var, T min, T max)
{
    if (var < min) {
        return min;
    } else if (var > max) {
        return max;
    } else {
        return var;
    }
}

class CollectibleType {
public:
    std::string name;
    std::string trigger_event;
    std::string collection_event;
    std::string missed_event;
    std::string on_show;
    std::string on_collect;
    std::string on_missed;
    float start_time;
    float duration;
    uint32_t goal;
    uint32_t collected;
    std::shared_ptr<olc::Sprite> sprite;
};

class Collectible {
public:
    bool visible;
    bool collected;
    std::shared_ptr<CollectibleType> type;
    int pos_x;
    int pos_y;
};

class Layer {
public:
    std::shared_ptr<olc::Sprite> background;
    std::shared_ptr<olc::Sprite> walk_mask;
    std::shared_ptr<olc::Sprite> light_mask;

    std::list<Collectible> collectibles;
};

class World {
public:
    std::vector<std::shared_ptr<CollectibleType>> collectible_types;

public:
    int width;
    int height;
    std::vector<Layer> layers;

public:
    std::shared_ptr<olc::Sprite> player;
    float pos_x;
    float pos_y;
    int layer;
    float time_remaining;

public:
    int viewport_x;
    int viewport_y;

public:
    void update_viewport(uint32_t screen_width, uint32_t screen_height)
    {
        int pos_x = std::round(this->pos_x);
        int pos_y = std::round(this->pos_y);

        int offset_x = screen_width / 2;
        int offset_y = screen_height / 2;

        viewport_x = clamp<int>(pos_x - offset_x, 0, width - screen_width);
        viewport_y = clamp<int>(pos_y - offset_y, 0, height - screen_height);
    }
};

class Outdoors : public olc::PixelGameEngine
{
public:
    static const uint8_t GS_INIT = 0;
    static const uint8_t GS_CREDITS = 1;
    static const uint8_t GS_TITLE = 2;
    static const uint8_t GS_MAIN = 3;
    static const uint8_t GS_WON = 4;
    static const uint8_t GS_LOST = 5;
    static const uint8_t GS_SLEEP = 6;
    static const uint8_t GS_PAUSE = 7;
    static const uint8_t GS_EXIT = 8;

    uint8_t game_state = 0;
    float timer = 0;

    std::unique_ptr<World> world = nullptr;
    float acc_x = 0;
    float acc_y = 0;
    int option = 0;

    void update_state(uint8_t new_state)
    {
        if (game_state != new_state) {
            game_state = new_state;
            timer = 0;
            acc_x = 0;
            acc_y = 0;
            option = 0;
        }
    }

public:
	Outdoors()
	{
		sAppName = "PaintedEggs";
	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
        uint8_t next_state;
        switch (game_state) {
            case GS_INIT:
                next_state = init();
                break;
            case GS_CREDITS:
                next_state = credits();
                break;
            case GS_TITLE:
                next_state = title();
                break;
            case GS_MAIN:
                next_state = main(fElapsedTime);
                break;
            case GS_WON:
                next_state = won();
                break;
            case GS_LOST:
                next_state = lost();
                break;
            case GS_SLEEP:
                next_state = sleep();
                break;
            case GS_PAUSE:
                next_state = pause();
                break;
            case GS_EXIT:
                return false;
        }

        timer += fElapsedTime;

        update_state(next_state);


		return true;
	}

	uint8_t init()
    {
        if (0 != timer) {
            std::vector<std::string> failed;
            failed.push_back("Could not load:");


            // Load all the things!
            world = std::make_unique<World>();
            world->height = 720;
            world->width = 1024;

            world->player = std::make_shared<olc::Sprite>();
            if (olc::OK != world->player->LoadFromFile("guy.png")) {
                failed.push_back("guy.png");
            }

            std::shared_ptr<CollectibleType> egg_type = std::make_shared<CollectibleType>();
            egg_type->name = "Eggs";
            egg_type->goal = 20;
            egg_type->sprite = std::make_shared<olc::Sprite>();
            if (olc::OK != egg_type->sprite->LoadFromFile("egg.png")) {
                failed.push_back("egg.png");
            }
            world->collectible_types.push_back(egg_type);

            // Layer 0
            {
                Layer layer;
                layer.background = std::make_shared<olc::Sprite>();
                ;
                if (olc::OK != layer.background->LoadFromFile("layers/background-1.png")) {
                    failed.push_back("layers/background-1.png");
                }
                layer.walk_mask = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.walk_mask->LoadFromFile("layers/walk-1.png")) {
                    failed.push_back("layers/walk-1.png");
                }

                std::vector<std::pair<int, int>> eggs = {
                    {295, 335},
                    {406, 267},
                    {442, 255},
                    {478, 307},
                    {498, 243},
                    {513, 161},
                    {560, 288},
                    {592, 312},
                    {594, 221}
                };

                for (auto coords: eggs) {
                    Collectible egg;
                    egg.pos_x = coords.first;
                    egg.pos_y = coords.second;
                    egg.type = egg_type;
                    layer.collectibles.push_back(egg);
                }

                world->layers.push_back(layer);
            }

            // Layer 1
            {
                Layer layer;
                layer.background = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.background->LoadFromFile("layers/background-2.png")) {
                    failed.push_back("layers/background-2.png");
                }
                layer.walk_mask = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.walk_mask->LoadFromFile("layers/walk-2.png")) {
                    failed.push_back("layers/walk-2.png");
                }

                std::vector<std::pair<int, int>> eggs = {
                    {  95, 495},
                    { 130, 425},
                    { 525, 350},
                    { 578, 440},
                    {1004, 470}
                };

                for (auto coords: eggs) {
                    Collectible egg;
                    egg.pos_x = coords.first;
                    egg.pos_y = coords.second;
                    egg.type = egg_type;
                    layer.collectibles.push_back(egg);
                }

                world->layers.push_back(layer);
            }

            // Layer 2
            {
                Layer layer;
                layer.background = std::make_shared<olc::Sprite>();
                ;
                if (olc::OK != layer.background->LoadFromFile("layers/background-3.png")) {
                    failed.push_back("layers/background-3.png");
                }
                layer.walk_mask = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.walk_mask->LoadFromFile("layers/walk-3.png")) {
                    failed.push_back("layers/walk-3.png");
                }

                std::vector<std::pair<int, int>> eggs = {
                    { 160, 620},
                    { 200, 585},
                    { 440, 554},
                    { 535, 655},
                    { 739, 647},
                    {1004, 637}
                };

                for (auto coords: eggs) {
                    Collectible egg;
                    egg.pos_x = coords.first;
                    egg.pos_y = coords.second;
                    egg.type = egg_type;
                    layer.collectibles.push_back(egg);
                }

                world->layers.push_back(layer);
            }

            // Layer 3
            {
                Layer layer;
                layer.background = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.background->LoadFromFile("layers/background-4.png")) {
                    failed.push_back("layers/background-4.png");
                }
                layer.walk_mask = std::make_shared<olc::Sprite>();
                if (olc::OK != layer.walk_mask->LoadFromFile("layers/walk-4.png")) {
                    failed.push_back("layers/walk-4.png");
                }

                std::vector<std::pair<int, int>> eggs = {
                };

                for (auto coords: eggs) {
                    Collectible egg;
                    egg.pos_x = coords.first;
                    egg.pos_y = coords.second;
                    egg.type = egg_type;
                    layer.collectibles.push_back(egg);
                }

                world->layers.push_back(layer);
            }

            if (1 != failed.size()) {
                Clear(olc::BLACK);
                for (int i = 0; i < failed.size(); i++) {
                    DrawString(4, i * 12 + 4, failed[i]);
                }
                return GS_INIT;
            } else {
                return GS_CREDITS;
            }
        }

        Clear(olc::BLACK);
        DrawString(8, ScreenHeight() - 16, "LOADING... please wait");
        return GS_INIT;
    }

	uint8_t credits()
    {
        if (GetKey(olc::ESCAPE).bPressed) {
            return GS_TITLE;
        }
        const float ENGINE_CREDITS = 3;
        const float BLANK_1 = ENGINE_CREDITS + 1;
        const float CREATOR_CREDITS = BLANK_1 + 3.5;
        const float BLANK_2 = CREATOR_CREDITS + 1;
        const float GAME_JAM_CREDITS = BLANK_2 + 4;
        const float BLANK_3 = GAME_JAM_CREDITS + 1;
        const float TITLE_CREDITS = BLANK_3 + 5;
        const float BLANK_4 = TITLE_CREDITS + 1.5;

        Clear(olc::BLACK);

        if (timer < ENGINE_CREDITS) {
            const std::string ENGINE = "olcPixelGameEngine";
            int offset_x = ENGINE.length() / 2 * 8;
            int offset_y = 4;
            DrawString(ScreenWidth() / 2 - offset_x, ScreenHeight() / 2 - offset_y, ENGINE);
        } else if (timer < BLANK_1) {
        } else if (timer < CREATOR_CREDITS) {
            DrawString(256/2-8*8, 240/2-8, "Dennis Bellinger");
            DrawString(256/2-4*8, 240/2, "Presents");
        } else if (timer < BLANK_2) {
        } else if (timer < GAME_JAM_CREDITS) {
            DrawString(ScreenWidth()/2 - 8, ScreenHeight()/2-12, "An");
            DrawString(ScreenWidth()/2 - 9*8-4, ScreenHeight()/2-4, "olc::BeatTheBoredom");
            DrawString(ScreenWidth()/2 - 7 * 8, ScreenHeight()/2+4, "Game Jam Entry");
        } else if (timer < BLANK_3) {
        } else if (timer < TITLE_CREDITS) {
            DrawString(ScreenWidth()/2-5*16-8, ScreenHeight()/2-8, "PaintedEggs", olc::WHITE, 2);
        } else if (timer < BLANK_4) {
        } else {
            return GS_TITLE;
        }

        return GS_CREDITS;
    }

	uint8_t title()
    {
        if (timer == 0) {
            world->time_remaining = 180;
            world->layer = 1;
            world->pos_x = 571;
            world->pos_y = 459;
            world->update_viewport(ScreenWidth(), ScreenHeight());
            for (auto &layer: world->layers) {
                for (auto &egg: layer.collectibles) {
                    egg.collected = false;
                    egg.visible = true;
                }
            }
            for (auto type: world->collectible_types) {
                type->collected = 0;
            }
        }
        if (GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            switch (option) {
                case 0:
                    return GS_MAIN;
                case 1:
                    return GS_EXIT;
            }
        }
        if (GetKey(olc::UP).bPressed || GetKey(olc::W).bPressed || GetKey(olc::K).bPressed) {
            option--;
        }
        if (GetKey(olc::DOWN).bPressed || GetKey(olc::S).bPressed || GetKey(olc::J).bPressed) {
            option++;
        }
        option = clamp<int>(option, 0, 1);

        if (timer > 30 || GetKey(olc::ESCAPE).bPressed) {
            return GS_SLEEP;
        }
        Clear(olc::BLACK);
        DrawString(ScreenWidth() / 2 - 5 * 16 - 8, ScreenHeight() / 4 - 8, "PaintedEggs", olc::WHITE, 2);
        DrawString(ScreenWidth() / 2 - 2 * 8 - 4, ScreenHeight() / 2 - 4, "Start");
        DrawString(ScreenWidth() / 2 - 2 * 8, ScreenHeight() / 2 + 4, "Exit");
        DrawString(ScreenWidth() / 2 - 4 * 8 - 4, ScreenHeight() / 2 - 4 + option * 8, "*");

        return GS_TITLE;
    }

	uint8_t main(float fElapsedTime)
    {
        if (GetKey(olc::ESCAPE).bPressed) {
            return GS_PAUSE;
        }

        const float PLAYER_SPEED = 60;
        const float STEP = PLAYER_SPEED * fElapsedTime;

        // 1. Move Player
        if (GetKey(olc::UP).bHeld || GetKey(olc::W).bHeld || GetKey(olc::K).bHeld) {
            acc_y -= STEP;
            while (-1 >= acc_y) {
                acc_y++;
                olc::Pixel p = world->layers[world->layer].walk_mask->GetPixel(world->pos_x, world->pos_y - 1);
                if (olc::BLUE == p) {
                    world->pos_y--;
                    world->layer--;
                } else if (olc::YELLOW == p) {
                    world->pos_y--;
                    world->layer++;
                } else if (olc::BLACK != p) {
                    world->pos_y--;
                }
            }
        }

        if (GetKey(olc::DOWN).bHeld || GetKey(olc::S).bHeld || GetKey(olc::J).bHeld) {
            acc_y += STEP;
            while (1 <= acc_y) {
                acc_y--;
                olc::Pixel p = world->layers[world->layer].walk_mask->GetPixel(world->pos_x, world->pos_y + 1);
                if (olc::BLUE == p) {
                    world->pos_y++;
                    world->layer--;
                } else if (olc::YELLOW == p) {
                    world->pos_y++;
                    world->layer++;
                } else if (olc::BLACK != p) {
                    world->pos_y++;
                }
            }
        }

        if (GetKey(olc::LEFT).bHeld || GetKey(olc::A).bHeld || GetKey(olc::H).bHeld) {
            acc_x -= STEP;
            while (-1 >= acc_x) {
                acc_x++;
                olc::Pixel p = world->layers[world->layer].walk_mask->GetPixel(world->pos_x - 1, world->pos_y);
                if (olc::BLUE == p) {
                    world->pos_x--;
                    world->layer--;
                } else if (olc::YELLOW == p) {
                    world->pos_x--;
                    world->layer++;
                } else if (olc::BLACK != p) {
                    world->pos_x--;
                }
            }
        }

        if (GetKey(olc::RIGHT).bHeld || GetKey(olc::D).bHeld || GetKey(olc::L).bHeld) {
            acc_x += STEP;
            while (1 <= acc_x) {
                acc_x--;
                olc::Pixel p = world->layers[world->layer].walk_mask->GetPixel(world->pos_x + 1, world->pos_y);
                if (olc::BLUE == p) {
                    world->pos_x++;
                    world->layer--;
                } else if (olc::YELLOW == p) {
                    world->pos_x++;
                    world->layer++;
                } else if (olc::BLACK != p) {
                    world->pos_x++;
                }
            }
        }

        world->pos_x = clamp<float>(world->pos_x, 0, world->width);
        world->pos_y = clamp<float>(world->pos_y, 0, world->height);

        // 2. Collect collectibles
        for (auto &collectible: world->layers[world->layer].collectibles) {
            if ((world->pos_x <= (collectible.pos_x + collectible.type->sprite->width))
                && (world->pos_x >= collectible.pos_x)
                && (world->pos_y <= (collectible.pos_y + collectible.type->sprite->height))
                && (world->pos_y >= collectible.pos_y)
                && !collectible.collected
            ) {
                collectible.collected = true;
                collectible.type->collected++;
            }
        }

        // 3. Check for win
        bool is_won = true;
        for (auto type: world->collectible_types) {
            if (0 < type->goal && type->collected < type->goal) {
                is_won = false;
            }
        }
        if (!is_won) {
            // 4. Update time
            world->time_remaining -= fElapsedTime;

            // 5. Check for loss
            if (0 >= world->time_remaining) {
                return GS_LOST;
            }

            // 5. Update Collectibles
        }


        // 6. Render World
        Clear(olc::BLACK);
        world->update_viewport(ScreenWidth(), ScreenHeight());
        SetPixelMode(olc::Pixel::MASK);
        for (int i = 0; i <= world->layer; i++) {
            DrawPartialSprite(0, 0, world->layers[i].background.get(), world->viewport_x, world->viewport_y, ScreenWidth(), ScreenHeight());
        }
        for (auto collectible : world->layers[world->layer].collectibles) {
            if (!collectible.collected && collectible.visible) {
                DrawSprite(collectible.pos_x - world->viewport_x, collectible.pos_y - world->viewport_y, collectible.type->sprite.get());
            }
        }
        DrawSprite(world->pos_x - world->viewport_x - 8, world->pos_y - world->viewport_y - 8, world->player.get());

        SetPixelMode(olc::Pixel::NORMAL);

        // Draw score
        {
            int line = 0;
            for (auto type: world->collectible_types) {
                if (0 < type->goal) {
                    std::stringstream ss;
                    ss << std::setw(4) << std::right << type->collected
                        << std::setw(0) << std::left << "/"
                        << std::setw(4) << std::right << type->goal
                        << std::setw(0) << std::left << " " << type->name;
                    DrawString(4, line * 8 + 4, ss.str());
                    line++;
                }
            }
        }

        // Draw Time
        {
            std::stringstream ss;
            int min = world->time_remaining / 60;
            int sec = world->time_remaining - min * 60;

            ss << std::setfill('0')
                << std::setw(2) << std::right << min
                << std::setw(0) << std::left << ":"
                << std::setw(2) << std::right << sec;

            DrawString(ScreenWidth() - 4 - ss.str().length() * 8, 4, ss.str());
        }

#ifndef NDEBUG
        // Draw coords
        {
            std::stringstream ss;
            ss << std::right << "("
                << std::setw(4) << std::to_string((int) world->pos_x)
                << std::setw(0) << ", "
                << std::setw(4) << std::to_string((int)world->pos_y)
                << std::setw(0) << ")"
                << std::endl;

            DrawString(4, ScreenHeight() - 3 * 8, ss.str());
        }
#endif

        if (is_won) {
            return GS_WON;
        } else {
            return GS_MAIN;
        }
    }

	uint8_t won()
    {

        if (GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            return GS_TITLE;
        }
        if (timer > 30) {
            return GS_SLEEP;
        }
        if (timer == 0) {
            DrawString(ScreenWidth()/2 - 3 * 24 - 12, ScreenHeight() / 4 - 12, "Winner!", olc::WHITE, 3);
            DrawString(ScreenWidth()/2 - 12 * 8 - 4, ScreenHeight() - 2 * 8, "Press SPACE to continue");
        }
        return GS_WON;
    }

    uint8_t lost()
    {
        if (GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            return GS_TITLE;
        }
        if (timer > 30) {
            return GS_SLEEP;
        }
        if (timer == 0) {
            DrawString(ScreenWidth()/2 - 3 * 24, ScreenHeight() / 4 - 12, "Loser!", olc::WHITE, 3);
            DrawString(ScreenWidth()/2 - 12 * 8 - 4, ScreenHeight() - 2 * 8, "Press SPACE to continue");
        }
        return GS_LOST;
    }

    uint8_t sleep()
    {
        if (GetKey(olc::ESCAPE).bPressed || GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            return GS_CREDITS;
        }
        Clear(olc::BLACK);

        SetPixelMode(olc::Pixel::MASK);
        DrawSprite(ScreenWidth() / 2 - 8, ScreenHeight() / 2 - 8, world->player.get());

        for (int i = 0; i < 6; i++) {
            DrawSprite(
                (sin(timer / 3 + i * 2 * PI / 6.0) / 2 + 0.5) * (ScreenWidth() - 32) + 8,
                (cos(timer / 3 + i * 2 * PI / 6.0) / 2 + 0.5) * (ScreenHeight() - 32) + 8,
                world->collectible_types[0]->sprite.get()
            );
        }
        SetPixelMode(olc::Pixel::NORMAL);

        return GS_SLEEP;
    }

    uint8_t pause()
    {
        if (GetKey(olc::ESCAPE).bPressed || GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            switch (option) {
                case 0:
                    return GS_MAIN;
                case 1:
                    return GS_TITLE;
            }
        }
        if (GetKey(olc::UP).bPressed || GetKey(olc::W).bPressed || GetKey(olc::K).bPressed) {
            option--;
        }
        if (GetKey(olc::DOWN).bPressed || GetKey(olc::S).bPressed || GetKey(olc::J).bPressed) {
            option++;
        }
        option = clamp<int>(option, 0, 1);
        if (timer > 300) {
            return GS_TITLE;
        }
        Clear(olc::BLACK);
        DrawString(ScreenWidth() / 2 - 3 * 8, ScreenHeight() / 4 - 4, "Paused");
        DrawString(ScreenWidth() / 2 - 3 * 8, ScreenHeight() / 2 - 4, "Resume");
        DrawString(ScreenWidth() / 2 - 2 * 8, ScreenHeight() / 2 + 4, "Exit");
        DrawString(ScreenWidth() / 2 - 5 * 8, ScreenHeight() / 2 - 4 + option * 8, "*");
        return GS_PAUSE;
    }
};


int main()
{
	Outdoors puzzle;
	if (puzzle.Construct(256, 240, 4, 4))
		puzzle.Start();

	return 0;
}
