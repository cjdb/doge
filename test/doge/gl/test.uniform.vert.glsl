# version 430

struct float_t {
   float a;
   float b;
   float c;
};

uniform float_t f;

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

float sum(const float a, const int b, const uint c)
{
   return a + float(b) + float(c);
}

void main()
{
   gl_Position = vec4(sum(f.a, i.a, u.a), sum(f.b, i.b, u.b), sum(f.c, i.c, u.c), 0.0)
               + vec4(v2.a, v2.b)
               + vec4(v2.c, iv2.a)
               + vec4(iv2.b, iv2.c);
}
