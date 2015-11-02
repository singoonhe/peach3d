/**
 * There is no need to set #version, predef have do it for OpenGL 3.x and OpenGL ES 3.0.
 * Predef have set precision highp float if supported.
 * You also can use system macros GL_ES to diff OpenGL and OpenGL ES.
 */

#ifdef PD_LEVEL_GL3
// global uniform block, include proj/view matrix and abiment
// this will effect all object
uniform GlobalUnifroms
{
    mat4 pd_projMatrix;
    mat4 pd_viewMatrix;
    vec4 pd_ambient;
};

// object uniform block, include model matrix and other attrs
// such as light, diffuse, spherical
uniform ObjectUnifroms
{
    mat4 pd_modelMatrix;
};

in vec3 pd_vertex;
in vec4 pd_color;
in vec2 pd_uv;
out vec4 out_color;
out vec2 out_uv;
#else
attribute vec3 pd_vertex;
attribute vec4 pd_color;
attribute vec2 pd_uv;
uniform mat4 pd_projMatrix;
uniform mat4 pd_viewMatrix;
uniform vec4 pd_ambient;
uniform mat4 pd_modelMatrix;
varying mediump vec4 out_color;
varying mediump vec2 out_uv;
#endif

void main(void)
{
#ifdef PD_LEVEL_GL3
    mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
	gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
    out_color = mix(pd_ambient, pd_color, pd_color.a);
    out_uv = pd_uv;
#else
    mat4 mvpMatrix = pd_projMatrix * pd_viewMatrix * pd_modelMatrix;
    gl_Position = mvpMatrix * vec4(pd_vertex, 1.0);
    out_color = mix(pd_ambient, pd_color, pd_color.a);
    out_uv = pd_uv;
#endif
}