# version 430

struct float_t {
   float a;
   float b;
   float c;
};

uniform float_t f;
uniform float_t bad_type;

struct integral_t {
   int a;
   int b;
   int c;
};

uniform integral_t i;

struct unsigned_t {
   uint a;
   uint b;
   uint c;
};

uniform unsigned_t u;

struct vec2_t {
   vec2 a;
   vec2 b;
   vec2 c;
};

uniform vec2_t v2;

struct ivec2_t {
   ivec2 a;
   ivec2 b;
   ivec2 c;
};

uniform ivec2_t iv2;

struct uvec2_t {
   uvec2 a;
   uvec2 b;
   uvec2 c;
};

uniform uvec2_t uv2;

struct vec3_t {
   vec3 a;
   vec3 b;
   vec3 c;
};

uniform vec3_t v3;

struct ivec3_t {
   ivec3 a;
   ivec3 b;
   ivec3 c;
};

uniform ivec3_t iv3;

struct uvec3_t {
   uvec3 a;
   uvec3 b;
   uvec3 c;
};

uniform uvec3_t uv3;

struct vec4_t {
   vec4 a;
   vec4 b;
   vec4 c;
};

uniform vec4_t v4;

struct ivec4_t {
   ivec4 a;
   ivec4 b;
   ivec4 c;
};

uniform ivec4_t iv4;

struct uvec4_t {
   uvec4 a;
   uvec4 b;
   uvec4 c;
};

uniform uvec4_t uv4;

void main()
{
   gl_Position = vec4(v2.a, v2.b * v2.c)
               + vec4(iv2.a, iv2.b * iv2.c)
               + vec4(uv2.a, uv2.b * uv2.c)
               + vec4(v3.a * v3.b * v3.c, f.a * f.b * f.c)
               + vec4(iv3.a * iv3.b * iv3.c, i.a * i.b * i.c)
               + vec4(uv3.a * uv3.b * uv3.c, u.a * u.b * u.c)
               + vec4(v4.a * v4.b * v4.c)
               + vec4(iv4.a * iv4.b * iv4.c)
               + vec4(uv4.a * uv4.b * uv4.c);
}
