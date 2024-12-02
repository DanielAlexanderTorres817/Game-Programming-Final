#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "Title.h"
#include "random"
#include "cstdlib"


// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
LevelA *g_level_a;
Title *g_title;
//LevelB* g_level_b;
//LevelC* g_level_c;
//Lose* g_lose;
//Win* g_win;
Scene* g_levels[3];

SDL_Window* g_display_window;
GLuint font;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

int lives = 3;
int P1lives = 3;
int P2lives = 3;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();



bool g_title_flag = true;



void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Platformer!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    
    //g_view_matrix = glm::translate(g_view_matrix, view_offset);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    
    
    // ————— LEVEL SETUP ————— //
    
    g_title = new Title();
    g_level_a = new LevelA();
    

    g_levels[0] = g_title;
    g_levels[1] = g_level_a; 
    

    


    switch_to_scene(g_levels[0]);
    font = Utility::load_texture("assets/font1.png");
    

    
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    //g_current_scene->get_state().P1cursor->set_rotation(glm::vec3(0.0f));
    //g_current_scene->get_state().P2cursor->set_rotation(glm::vec3(0.0f));
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;

                //case SDLK_SPACE:
                //    // ————— JUMPING ————— //
                //    if (g_current_scene->get_state().player->get_collided_bottom())
                //    {
                //        g_current_scene->get_state().player->jump();
                //        Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                //    }
                     //break;

            default:
                break;
            }
            /* case SDL_KEYUP:
                 switch (event.key.keysym.sym) {
                     case SDLK_SPACE:
                         g_current_scene->get_state().fireball1->move_right();

                 }
                 default:
                     break;
         }*/
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A]) {
        g_current_scene->get_state().player->move_left();
        g_current_scene->get_state().player->face_left();
        g_current_scene->get_state().player->set_texture_id(WALK);
        g_current_scene->get_state().player->set_animation_state(WALK);

    }
    else if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D]) {
        g_current_scene->get_state().player->move_right();
        g_current_scene->get_state().player->face_right();
        g_current_scene->get_state().player->set_texture_id(WALK);
        g_current_scene->get_state().player->set_animation_state(WALK);
    }

    else {
        g_current_scene->get_state().player->set_texture_id(IDLE);
        g_current_scene->get_state().player->set_animation_state(IDLE);
    }

    if (key_state[SDL_SCANCODE_RETURN] && g_title_flag) {
        g_title_flag = false;
        
        switch_to_scene(g_levels[1]);
    }
    /*if (key_state[SDL_SCANCODE_K]) {
        lives = 0;
    }*/
    if (key_state[SDL_SCANCODE_J]) {
        g_current_scene->get_state().map->win_status = true;
        //switch_to_scene(g_levels[5]);
    }
    if (!g_title_flag && key_state[SDL_SCANCODE_UP]) {
        g_current_scene->get_state().P1cursor->rotate_ccw();
    }
    else if (!g_title_flag && key_state[SDL_SCANCODE_DOWN]) {
        g_current_scene->get_state().P1cursor->rotate_cw();
    }
    if (!g_title_flag && key_state[SDL_SCANCODE_SPACE] && !g_current_scene->get_state().fireball1->shoot_flag) {
        //g_current_scene->get_state().P1cursor->activate();
        g_current_scene->get_state().fireball1->activate();
        float x = glm::cos((g_current_scene->get_state().P1cursor->get_rotation()));
        float y = glm::sin((g_current_scene->get_state().P1cursor->get_rotation()));
        


        
       
        g_current_scene->get_state().fireball1->jump();
        g_current_scene->get_state().fireball1->set_movement(glm::vec3(x, 0.0f, 0.0f));
        //g_current_scene->get_state().fireball1->set_velocity(glm::vec3(1.0f, 1.0f, 0.0f));
        //g_current_scene->get_state().fireball1->set_acceleration(glm::vec3(0.0f, -4.5f, 0.0f));
        

    }
    else if (!g_title_flag){
        //g_current_scene->get_state().P1cursor->deactivate();
        //g_current_scene->get_state().fireball->deactivate();
    }
     
    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
 
}

void update()
{
    if (g_title_flag) { return; }
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;

    
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        //if (g_current_scene->get_state().subtract) {
        //    lives--; 
        //    g_current_scene->get_state().subtract = false;
        //}

        //if (lives <= 0) {
        //    g_current_scene->get_state().end = true;
        //}

        //if (g_current_scene->get_state().end == true) {
        //    //switch_to_scene(g_levels[4]);
        //}

        //if (g_current_scene->get_state().map->win_status == true) {
        //    //switch_to_scene(g_levels[5]);
        //}

        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    //g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));

    
    if (g_current_scene->get_state().fireball1->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().fireball1->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
}

void render()
{
    
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    if (!g_title_flag) {
        Utility::draw_text(&g_shader_program, font, "LIVES:" + std::to_string(P1lives), 0.3f, 0.0f,
            g_current_scene->get_state().player->get_position() + glm::vec3(-2.5f, 4.75f, 0.0f));

    }
    


    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_title;
    //delete g_level_b;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();

        if (g_current_scene->get_state().next_scene_id != 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);

        render();
        
    }
    
    shutdown();
    return 0;
}
