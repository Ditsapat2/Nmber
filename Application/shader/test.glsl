#shader vertex
#version 330 core
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_proj;
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in float texID;
layout (location = 3) in vec2 texCoord;

out float v_texID;
out vec2 v_texCoord;
out vec4 v_color;

void main()
{
    mat4 mat = u_proj*u_view*u_model;
    gl_Position = mat*position;
    v_color = color;
    v_texID = texID;
    v_texCoord = texCoord;
}

#shader fragment
uniform sampler2D u_textures[8];
in float v_texID;
in vec2 v_texCoord;
in vec4 v_color;
out vec4 FragColor;

void main()
{
    if (v_texID>=0.0) FragColor =  texture2D(u_textures[int(v_texID)], v_texCoord)*v_color;
    else FragColor = v_color;
}
