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
#include "P1win.h"
#include "P2win.h"
#include "LevelB.h"
#include "LevelC.h"
#include "random"
#include "cstdlib"
#include "iostream"





// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640 * 1.5,
WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };
enum Turn { PLAYER1, PLAYER2 };

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Turn turn = PLAYER1;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
Title* g_title;
P1win* g_p1win;
P2win* g_p2win;

Scene* g_levels[6];

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


void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update(int random_number);
void render();
void shutdown();



bool g_title_flag = true;

float left_offset = -(LEVEL_WIDTH * 0.5) / 2.0f;
float top_offset = (LEVEL_HEIGHT * 0.5) / 2.0f;
glm::vec3 g_view_offset(left_offset, top_offset, 0.0f);



void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Tower Seige!",
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
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_p1win = new P1win();
    g_p2win = new P2win();


    g_levels[0] = g_title;
    g_levels[1] = g_level_a;
    g_levels[2] = g_p1win;
    g_levels[3] = g_p2win;
    g_levels[4] = g_level_b;
    g_levels[5] = g_level_c;





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
            case SDLK_h:
                
                if (turn == PLAYER1) {
                    g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
                    g_current_scene->get_state().fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball1->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball1->deactivate();
                    turn = PLAYER2;
                }
                else {
                    g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
                    g_current_scene->get_state().fireball2->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball2->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball2->deactivate();
                    turn = PLAYER1;
                }

                
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
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    /*if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A]) {
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
    }*/

    if (key_state[SDL_SCANCODE_1] && g_title_flag) {
        g_title_flag = false;

        switch_to_scene(g_levels[1]);
    }
    if (key_state[SDL_SCANCODE_2] && g_title_flag) {
        g_title_flag = false;

        switch_to_scene(g_levels[4]);
    }
    if (key_state[SDL_SCANCODE_3] && g_title_flag) {
        g_title_flag = false;

        switch_to_scene(g_levels[5]);
    }
    /*if (key_state[SDL_SCANCODE_K]) {
        lives = 0;
    }*/
    
    if (key_state[SDL_SCANCODE_J]) {
        g_current_scene->get_state().map->win_status = true;
        //switch_to_scene(g_levels[5]);
    }
    if (!g_title_flag && key_state[SDL_SCANCODE_W]) {
        g_current_scene->get_state().P1cursor->rotate_ccw();
    }
    else if (!g_title_flag && key_state[SDL_SCANCODE_S]) {
        g_current_scene->get_state().P1cursor->rotate_cw();
    }
    if (!g_title_flag && key_state[SDL_SCANCODE_UP]) {
        g_current_scene->get_state().P2cursor->rotate_cw();
    }
    else if (!g_title_flag && key_state[SDL_SCANCODE_DOWN]) {
        g_current_scene->get_state().P2cursor->rotate_ccw();
    }
    if (!g_title_flag && key_state[SDL_SCANCODE_LSHIFT] && !g_current_scene->get_state().fireball1->shoot_flag) {

        if (turn == PLAYER1) {
            g_current_scene->get_state().fireball1->activate();
            float x = glm::cos((g_current_scene->get_state().P1cursor->get_rotation()));
            float y = glm::sin((g_current_scene->get_state().P1cursor->get_rotation()));

            g_current_scene->get_state().fireball1->jump();
            g_current_scene->get_state().fireball1->set_movement(glm::vec3(x, y + 0.5f, 0.0f));
            //g_current_scene->get_state().fireball1->shoot_flag = true;
        }
        //g_current_scene->get_state().P1cursor->activate();

        //g_current_scene->get_state().fireball1->set_velocity(glm::vec3(1.0f, 1.0f, 0.0f));
        //g_current_scene->get_state().fireball1->set_acceleration(glm::vec3(0.0f, -4.5f, 0.0f));


    }
    else if (!g_title_flag && key_state[SDL_SCANCODE_SPACE] && !g_current_scene->get_state().fireball2->shoot_flag) {
        if (turn == PLAYER2) {
            g_current_scene->get_state().fireball2->activate();
            float x = glm::cos((g_current_scene->get_state().P2cursor->get_rotation()));
            float y = glm::sin((g_current_scene->get_state().P2cursor->get_rotation()));

            g_current_scene->get_state().fireball2->jump();
            g_current_scene->get_state().fireball2->set_movement(glm::vec3(x, y + 0.5f, 0.0f));
        }
    }


    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();

}

void update(int random_number)
{

    if (g_title_flag) { return; }
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    float fb1_prev_x = g_current_scene->get_state().fireball1->get_position().x;
    float fb2_prev_x = g_current_scene->get_state().fireball2->get_position().x;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        float fb1_cur_x = g_current_scene->get_state().fireball1->get_position().x;
        float fb2_cur_x = g_current_scene->get_state().fireball2->get_position().x;

        if (g_current_scene->get_state().map->status != 'F' || g_current_scene->get_state().towers->status != 'F') {
            if (g_current_scene->get_state().map->status == 'H' || g_current_scene->get_state().towers->status == 'H') {
                if (turn == PLAYER1) {
                    P2lives--;
                    g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
                    g_current_scene->get_state().fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball1->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball1->deactivate();
                    turn = PLAYER2;
                }
                else if (turn == PLAYER2) {
                    P1lives--;
                    g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
                    g_current_scene->get_state().fireball2->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball2->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().fireball2->deactivate();
                    turn = PLAYER1;

                }
                
            }
            
            

          
            
            /*else  {
                if (turn == PLAYER1) {

                    g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
                    g_current_scene->get_state().fireball1->deactivate();
                    turn = PLAYER2;
                }
                else if (turn == PLAYER2) {

                    g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
                    g_current_scene->get_state().fireball2->deactivate();
                    turn = PLAYER1;

                }
            }*/

            g_current_scene->get_state().map->status = 'F';
            g_current_scene->get_state().towers->status = 'F';

        }
        if (g_current_scene->get_state().fireball1->get_position().x > 23.0f) {
            g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
            g_current_scene->get_state().fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball1->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball1->deactivate();
            turn = PLAYER2;
        }

        if (g_current_scene->get_state().fireball2->get_position().x < -12.0f) {
            g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
            g_current_scene->get_state().fireball2->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball2->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball2->deactivate();
            turn = PLAYER1;
        }

        if (g_current_scene->get_state().fireball1->m_is_active && fb1_prev_x == fb1_cur_x) {
            g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
            g_current_scene->get_state().fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball1->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball1->deactivate();
            turn = PLAYER2;
        }
        
        if (g_current_scene->get_state().fireball2->m_is_active && fb2_prev_x == fb2_cur_x) {
            g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
            g_current_scene->get_state().fireball2->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball2->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
            g_current_scene->get_state().fireball2->deactivate();
            turn = PLAYER1;
        }




        delta_time -= FIXED_TIMESTEP;

        

    }

    g_accumulator = delta_time;
    if (g_current_scene->get_state().player->bird_hit) {
        

        if (random_number == 1){
            g_title_flag = true;
            P2lives = 0;
        }
        else {
            g_title_flag = true;
            P1lives = 0;
        }
    }



    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    //g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));

    if (turn == PLAYER1) {
        if (g_current_scene->get_state().fireball1->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().fireball1->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
    else if (turn == PLAYER2) {
        if (g_current_scene->get_state().fireball2->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().fireball2->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
    if (P2lives == 0) {
        g_title_flag = true;
        switch_to_scene(g_levels[2]);

    }
    if (P1lives == 0) {
        g_title_flag = true;
        switch_to_scene(g_levels[3]);

    }
    /*if (g_current_scene->get_state().player->check_collision(g_current_scene->get_state().fireball2)) {
        std::cout << "COLLIDED WITH P1" << std::endl;
        

        g_current_scene->get_state().fireball1->set_position(g_current_scene->get_state().fireball1->get_starting_pos());
        g_current_scene->get_state().fireball1->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        g_current_scene->get_state().fireball1->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        g_current_scene->get_state().fireball1->deactivate();
        turn = PLAYER2;
    }
    if (g_current_scene->get_state().enemy->check_collision(g_current_scene->get_state().fireball1)) {
        std::cout << "COLLIDED WITH P2" << std::endl;
       
        g_current_scene->get_state().fireball2->set_position(g_current_scene->get_state().fireball2->get_starting_pos());
        g_current_scene->get_state().fireball2->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        g_current_scene->get_state().fireball2->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        g_current_scene->get_state().fireball2->deactivate();
        turn = PLAYER1;
    }*/


}

void render()
{

    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    if (!g_title_flag) {
        Utility::draw_text(&g_shader_program, font, "LIVES:" + std::to_string(P1lives), 0.3f, 0.0f,
            g_current_scene->get_state().player->get_position() + glm::vec3(-2.5f, 4.25f, 0.0f));

        Utility::draw_text(&g_shader_program, font, "LIVES:" + std::to_string(P2lives), 0.3f, 0.0f,
            g_current_scene->get_state().enemy->get_position() + glm::vec3(-0.5f, 4.25f, 0.0f));

        /*Utility::draw_text(&g_shader_program, font, "TURN:" + std::to_string(turn), 0.3f, 0.0f,
            g_current_scene->get_state().player->get_position() + glm::vec3(-0.5f, 4.25f, 0.0f));

        Utility::draw_text(&g_shader_program, font, "TURN:" + std::to_string(turn), 0.3f, 0.0f,
            g_current_scene->get_state().enemy->get_position() + glm::vec3(-0.5f, 4.25f, 0.0f));*/
    }



    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_title;
    delete g_p1win;
    delete g_p2win;
    delete g_level_b;
    delete g_level_c;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    std::srand(std::time(0));
    int random_number = 1 + std::rand() % 2;
    

    while (g_app_status == RUNNING)
    {
        process_input();
        update(random_number);



        if (g_current_scene->get_state().next_scene_id != 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);

        render();

    }

    shutdown();
    return 0;
}