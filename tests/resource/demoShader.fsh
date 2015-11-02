/**
 * There is no need to set #version, predef have do it for OpenGL 3.x and OpenGL ES 3.0.
 * Predef have set precision highp float if supported.
 * You also can use system macros GL_ES to diff OpenGL and OpenGL ES.
 */

#ifdef PD_LEVEL_GL3
in vec4 out_color;
in vec2 out_uv;
uniform sampler2D pd_texture0;

out vec4 out_FragColor;
#else
uniform sampler2D pd_texture0;
varying mediump vec2 out_uv;
varying mediump vec4 out_color;
#endif

// void main can't be include #if ... #end, android 2.0 compile error
void main(void)
 {
#ifdef PD_LEVEL_GL3
     vec4 tex_color = texture( pd_texture0, out_uv );
     out_FragColor = mix(out_color, tex_color, tex_color.a);
#else
     vec4 tex_color = texture2D( pd_texture0, out_uv );
     gl_FragColor = mix(out_color, tex_color, tex_color.a);
#endif
}
	
