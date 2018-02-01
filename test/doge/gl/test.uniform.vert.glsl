# version 430

struct float_t {
   float a;
   float b;
};

uniform float_t f;

struct integral_t {
   int a;
   int b;
};

uniform integral_t i;

struct unsigned_t {
   uint a;
   uint b;
};

uniform unsigned_t u;

float sum(const float a, const int b, const uint c)
{
   return a + float(b) + float(c);
}

void main()
{
   gl_Position = vec4(sum(f.a, i.a, u.a), sum(f.b, i.b, u.b), 0.0, 0.0);
}
