/**
 * There is no need to set #version, predef have do it for OpenGL 3.x and OpenGL ES 3.0.
 * Predef have set precision highp float if supported.
 * You also can use system macros GL_ES to diff OpenGL and OpenGL ES.
 */

#ifdef PD_LEVEL_GL3
// widget uniform block, include model matrix and other attrs
in mat4 pd_modelMatrix;
in vec2 pd_vertex;
in vec2 pd_uv;
in vec4 pd_color;
out vec2 out_uv;
out vec4 out_color;
#else
attribute vec2 pd_vertex;
attribute vec2 pd_uv;
attribute vec4 pd_color;
varying mediump vec2 out_uv;
varying mediump vec4 out_color;
#endif

void main(void)
{
#ifdef PD_LEVEL_GL3
    // pd_modelMatrix *
    gl_Position = pd_modelMatrix * vec4(pd_vertex, 0.0, 1.0);;
    out_uv = pd_uv;
    out_color = pd_color;
#else
    gl_Position = vec4(pd_vertex, 0.0, 1.0);
    out_uv = pd_uv;
    out_color = pd_color;
#endif
}