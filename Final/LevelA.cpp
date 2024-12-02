#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 40
#define LEVEL_HEIGHT 16

constexpr char PLAYER_IDLE_FILEPATH[] = "assets/player/Idle.png",
PLAYER_RUN_FILEPATH[] = "assets/player/Attack_2.png",
PLATFORM_FILEPATH[] = "assets/tilesheet.png",
ENEMY_FILEPATH[] = "assets/enemy/Idle.png",
TOWER_FILEPATH[] = "assets/tilemap_packed.png";

unsigned int LEVEL_DATA[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
    14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16
};


unsigned int TOWER_DATA[] =
{
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71,  4, 26, 26,  5, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  4, 26, 26,  5, 71,
    71, 16,  6,  6, 17, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 16,  6,  6, 17, 71,
    71, 57, 28, 29, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 28, 29, 59, 71,
    71, 57, 40, 40, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 40, 40, 59, 71,
    71, 57, 40, 40, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 40, 40, 59, 71,
    71, 57, 40, 28, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 40, 28, 59, 71,
    71, 57, 40, 40, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 40, 40, 59, 71,
    71, 57, 40, 40, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 40, 40, 59, 71,
    71, 57, 46, 47, 59, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 57, 46, 47, 59, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71
};



// tower = 11 rows, 12 cols


// ------ CAMERA ADJUST ---------//
float left_bound_offset = -(LEVEL_WIDTH * 0.5) / 2.0f;
float top_bound_offset = (LEVEL_HEIGHT * 0.5) / 2.0f;
glm::vec3 view_offset(left_bound_offset, top_bound_offset, 0.0f);

GLuint  A_font_texture_id;


LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    A_font_texture_id = Utility::load_texture("assets/font1.png");

    GLuint map_texture_id = Utility::load_texture(PLATFORM_FILEPATH);
    GLuint tower_texture_id = Utility::load_texture(TOWER_FILEPATH);

    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 0.5f, 7, 6);
    m_game_state.towers = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, TOWER_DATA, tower_texture_id, 0.5f, 12, 11);
    GLuint player_idle_texture_id = Utility::load_texture(PLAYER_IDLE_FILEPATH);
    GLuint player_walk_texture_id = Utility::load_texture(PLAYER_RUN_FILEPATH);


    // ------------ PLAYER 1 ------------------//
    std::vector<GLuint> player_texture_ids = {
        player_idle_texture_id,   // IDLE spritesheet
        player_walk_texture_id,   // WALK spritesheet
        //player_dead_texture_id, //DEAD spritesheet
        //player_attack_texture_id //attack spritesheet
    };


    std::vector<std::vector<int>> player_animations = {
        {0, 1, 2, 3, 4, 5, 6, 7},       // IDLE animation frames
        {0, 1, 2, 3, 4, 5, 6, 7, 8},       // RUN animation frames
        //{0, 1, 2, 3},                   // DEAD animation frames
        //{0, 1, 2, 3, 4, 5, 6, 7, 8}     // ATTACK animation frames
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -5.905f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_ids,        // texture id
        2.5f,                      // speed
        acceleration,              // acceleration
        4.5f,                      // jumping power
        player_animations,         // animation index sets
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        1,                         // animation row amount
        0.15f,                     // width
        1.5f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_scale(glm::vec3(1.5f, 1.5f, 1.0f));
    m_game_state.player->set_position(-view_offset + (glm::vec3(-7.3f, 0.0f, 0.0f)));

    
    //------------- CURSORS --------------//
    m_game_state.P1cursor = new Entity(Utility::load_texture("assets/arrow.png"), 0.0f, 0.0f, 0.0f, CURSOR);
    m_game_state.P2cursor = new Entity(Utility::load_texture("assets/arrow.png"), 1.0f, 1.0f, 1.0f, CURSOR);

    //m_game_state.P1cursor->set_texture_id_single(Utility::load_texture("assets/arrow.png"));
    //m_game_state.P2cursor->set_texture_id_single(Utility::load_texture("assets/arrow.png"));

    m_game_state.P1cursor->set_position(-view_offset + (glm::vec3(-6.0f, -1.0f, 0.0f)));
    m_game_state.P2cursor->set_position(-view_offset + (glm::vec3(6.0f, -1.0f, 0.0f)));
    
    m_game_state.P1cursor->set_scale(glm::vec3(1.75f, 0.75f, 1.0f));
    m_game_state.P2cursor->set_scale(glm::vec3(0.75f, 0.75f, 1.0f));

    //m_game_state.P2cursor->face_left();

    m_game_state.P1cursor->is_animated = false;
    m_game_state.P2cursor->is_animated = false;

    //m_game_state.P1cursor->deactivate();
    m_game_state.P2cursor->deactivate();

    //---------------------FIREBALLS----------------//
    m_game_state.fireball1 = new Entity(Utility::load_texture("assets/fireball.png"), 5.5f, 0.5f, 0.5f, FIREBALL);

    m_game_state.fireball1->set_position(-view_offset + (glm::vec3(-6.5f, -0.5f, 0.0f)));
    m_game_state.fireball1->set_scale(glm::vec3(0.5f, 0.25f, 1.0f));
    m_game_state.fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.fireball1->set_acceleration(glm::vec3(0.0f, -4.0f, 0.0f));
    m_game_state.fireball1->set_jumping_power(1);
    m_game_state.fireball1->deactivate();

    

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    std::vector<GLuint> enemy_texture_ids = {

        Utility::load_texture("assets/enemy/Idle.png"), // IDLE spritesheet
        Utility::load_texture("assets/enemy/Run.png"),  // WALK spritesheet


    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];
    std::vector<std::vector<int>> enemy_animations = {

        {0, 1, 2, 3, 4, 5, 6,},          // IDLE animation frames
        {0, 1, 2, 3, 4, 5, 6, 7},        // RUN animation frames


    };

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_ids, enemy_animations, 1.5f, 0.25f, 1.5f, ENEMY, GUARD, IDLING);
        m_game_state.enemies[i].set_scale(glm::vec3(1.5f, 1.5f, 1.0f));
    }


    m_game_state.enemies[0].set_position(glm::vec3(20.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    /**
     BGM and SFX
     */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/bgm.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, m_game_state.towers);


    for (int i = 0; i < ENEMY_COUNT; i++) {
        Entity& enemy = m_game_state.enemies[i];
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map, m_game_state.towers);

        
    }
    if (m_game_state.player->get_position().y < -7.0f) {
        m_game_state.next_scene_id = 2;
        m_game_state.change = true;
    }
    m_game_state.P1cursor->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, m_game_state.towers);
    m_game_state.P2cursor->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, m_game_state.towers);

    m_game_state.fireball1->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, m_game_state.towers);
}



void LevelA::render(ShaderProgram *g_shader_program)
{
    
    m_game_state.map->render(g_shader_program);
    m_game_state.towers->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.P1cursor->render(g_shader_program);
    m_game_state.P2cursor->render(g_shader_program);
    m_game_state.fireball1->render(g_shader_program);

    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(g_shader_program);

    
}
