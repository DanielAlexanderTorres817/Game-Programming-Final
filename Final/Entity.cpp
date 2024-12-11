#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;
            
        case JUMPER:
            //set_jumping_power(3.0f);
            if (get_collided_bottom()) { jump(); }
            break;
        case NONE:
            break;
        default:
            break;
    }
}

void Entity::ai_walk()
{
    float bottom_ref = m_starting_pos.y;
    float top_ref = m_starting_pos.y + 2.0;

    if (m_position.y == bottom_ref) {
        m_velocity = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    else if (m_position.y == top_ref) {
        m_velocity = glm::vec3(0.0f, -1.0f, 0.0f);
    }
    //else { m_movement = glm::vec3(0.0f, 1.0f, 0.0f); }
}

void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state) {
    case IDLING:
        if (glm::distance(m_position, player->get_position()) < 3.0f) {
            m_ai_state = WALKING;
        }
        break;
    case WALKING:
        ai_walk();
        break;

    case ATTACKING:
        break;

    default:
        break;
    }
}
// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_ids({}), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    /*for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0*/;
}

// Parameterized constructor
Entity::Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, float jump_power, std::vector<std::vector<int>> animations_indicies, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_ids(texture_ids), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    m_texture_id = m_texture_ids[IDLE];
    m_animations = animations_indicies;
    set_animation_state(IDLE);
    m_animation_indices = m_animations[IDLE].data();


}

// Simpler constructor for partial initialization for fireball and cursors
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType)
{
    //m_is_active = false;

}


Entity::Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations_indicies, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState) : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(7), m_animation_frames(7), m_animation_index(0),
m_animation_rows(1), m_animation_time(0.0f),
m_texture_ids(texture_ids), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    m_texture_id = m_texture_ids[IDLE];
    m_animations = animations_indicies;
    //m_animation_indices = m_animations[IDLE].data();
    set_animation_state(IDLE);

}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* shaderProgram, GLuint texture_id, int index, int rows, int cols)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % cols) / (float)cols;
    float v_coord = (float)(index / cols) / (float)rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(shaderProgram->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices);
    glEnableVertexAttribArray(shaderProgram->get_position_attribute());

    glVertexAttribPointer(shaderProgram->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        tex_coords);
    glEnableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(shaderProgram->get_position_attribute());
    glDisableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());
}



bool const Entity::check_collision(Entity* other) const
{
    if (other->m_is_active == false) { return false; }
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y -= y_overlap;
                m_velocity.y = 0;

                // Collision!
                m_collided_top = true;
            }
            else if (m_velocity.y < 0)
            {
                m_position.y += y_overlap;
                m_velocity.y = 0;

                // Collision!
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x -= x_overlap;
                m_velocity.x = 0;

                // Collision!
                m_collided_right = true;

            }
            else if (m_velocity.x < 0)
            {
                m_position.x += x_overlap;
                m_velocity.x = 0;

                // Collision!
                m_collided_left = true;
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}
void Entity::set_animation_state(Animation new_animation)
{
    m_current_animation = new_animation;

    // Update the texture and animation indices based on the current animation
    m_animation_indices = m_animations[m_current_animation].data();

    // Update the number of rows to match the new texture spritesheet
    m_animation_rows = 1;
    m_animation_cols = m_animations[m_current_animation].size();
    m_animation_frames = m_animations[m_current_animation].size();
    //m_animation_index = 0;

    // Since the attacking animation spritesheet is twice as long as
    // the idle animation, we scale accordingly
    /*switch (m_current_animation)
    {
        case IDLE:
            if (m_scale.x != m_scale.y) m_scale.x = m_scale.y;
            break;
        case WALK:
            if (m_scale.x < m_scale.y * 2) m_scale.x *= 2;

        default:
            break;
    }*/
}


void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map, Map* tower)
{
    if (!m_is_active) { return; };


    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;



    if (is_animated && m_animation_indices != NULL)
    {

        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second)
        {
            m_animation_time = 0.0f;
            m_animation_index++;
            if (m_current_animation == WALK && m_animation_index >= m_animation_frames -1) {
                m_animation_index = 5;
            }
            else if (m_animation_index >= m_animation_frames)
            {
                m_animation_index = 0;
            }
        }

    }
    //if (m_entity_type == BLOCKER) (ai_walk());
    m_velocity.x = m_movement.x * m_speed;
    //m_velocity += m_acceleration * delta_time;

    //m_velocity.y = m_movement.y * m_speed;
    m_velocity += m_acceleration * delta_time;


    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    check_collision_y(tower);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    check_collision_x(tower);

    if (m_entity_type == ENEMY) { ai_activate(player); };

    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }

    //m_rotation.z *= delta_time;
    if (m_entity_type == BLOCKER) {
        float top = m_starting_pos.y + 2.0f;
        float bottom = m_starting_pos.y;
        if (m_position.y <= bottom ) {
            m_velocity = glm::vec3(0.0f, 1.0f,0.0f);
        }
        else if (m_position.y >= top) {
            m_velocity = glm::vec3(0.0f, -1.0f, 0.0f);
        }
    }

    if (m_entity_type == BIRB) {
        float left = m_starting_pos.x;
        float right = m_starting_pos.x + 13.0f;
        if (m_position.x <= left) {
            m_velocity = glm::vec3(1.0f, 0.0f, 0.0f);
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            m_moving_left = true;
        }
        else if (m_position.x >= right) {
            m_velocity = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_moving_left = false;
        }
    }
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

    if (m_entity_type == CURSOR) {
        m_model_matrix = glm::rotate(m_model_matrix, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    if (m_moving_left) { m_model_matrix = glm::rotate(m_model_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); }

    
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);
    if (!m_is_active) return;

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_index, m_animation_rows, m_animation_cols);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}