/**
 * There is no need to set #version, predef have do it for OpenGL 3.x and OpenGL ES 3.0.
 * Predef have set precision highp float if supported.
 * You also can use system macros GL_ES to diff OpenGL and OpenGL ES.
 */

#ifdef PD_LEVEL_GL3
in vec2 out_uv;
in vec4 out_color;
uniform sampler2D pd_texture0;

out vec4 out_FragColor;
#else
varying mediump vec2 out_uv;
varying mediump vec4 out_color;
uniform sampler2D pd_texture0;
#endif

// void main can't be include #if ... #end, android 2.0 compile error
void main(void)
 {
#ifdef PD_LEVEL_GL3
     out_FragColor = texture( pd_texture0, out_uv ) * out_color;
#else
     gl_FragColor = texture2D( pd_texture0, out_uv ) * out_color;
#endif
}
	
