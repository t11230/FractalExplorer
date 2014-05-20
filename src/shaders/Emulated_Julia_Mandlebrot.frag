#version 130

// turn off NVIDIA compiler optimizations. Double emulation doesn't work otherwise.
#pragma optionNV(fastmath off)
#pragma optionNV(fastprecision off)

uniform float MaxIterations;
uniform float Zoom;
uniform float Xcenter;
uniform float Ycenter;
uniform float JuliaA;
uniform float JuliaB;

uniform float R;
uniform float G;
uniform float B;

// Interface Parameters
uniform bool Almond;
uniform bool LogShading;
uniform bool Julia;

out vec4 FragColor;

///////////////////////////
// Double Precision Math //
///////////////////////////
// Emulation based on Fortran-90 double-single package. See http://crd.lbl.gov/~dhbailey/mpdist/
// Substract: res = ds_add(a, b) => res = a + b
vec2 ds_add (vec2 dsa, vec2 dsb)
{
vec2 dsc;
float t1, t2, e;

 t1 = dsa.x + dsb.x;
 e = t1 - dsa.x;
 t2 = ((dsb.x - e) + (dsa.x - (t1 - e))) + dsa.y + dsb.y;

 dsc.x = t1 + t2;
 dsc.y = t2 - (dsc.x - t1);
 return dsc;
}

// Substract: res = ds_sub(a, b) => res = a - b
vec2 ds_sub (vec2 dsa, vec2 dsb)
{
vec2 dsc;
float e, t1, t2;

 t1 = dsa.x - dsb.x;
 e = t1 - dsa.x;
 t2 = ((-dsb.x - e) + (dsa.x - (t1 - e))) + dsa.y - dsb.y;

 dsc.x = t1 + t2;
 dsc.y = t2 - (dsc.x - t1);
 return dsc;
}

// Compare: res = -1 if a < b
//              = 0 if a == b
//              = 1 if a > b
float ds_compare(vec2 dsa, vec2 dsb)
{
 if (dsa.x < dsb.x) return -1.;
 else if (dsa.x == dsb.x) 
  {
  if (dsa.y < dsb.y) return -1.;
  else if (dsa.y == dsb.y) return 0.;
  else return 1.;
  }
 else return 1.;
}

// Multiply: res = ds_mul(a, b) => res = a * b
vec2 ds_mul (vec2 dsa, vec2 dsb)
{
vec2 dsc;
float c11, c21, c2, e, t1, t2;
float a1, a2, b1, b2, cona, conb, split = 8193.;

 cona = dsa.x * split;
 conb = dsb.x * split;
 a1 = cona - (cona - dsa.x);
 b1 = conb - (conb - dsb.x);
 a2 = dsa.x - a1;
 b2 = dsb.x - b1;

 c11 = dsa.x * dsb.x;
 c21 = a2 * b2 + (a2 * b1 + (a1 * b2 + (a1 * b1 - c11)));

 c2 = dsa.x * dsb.y + dsa.y * dsb.x;

 t1 = c11 + c2;
 e = t1 - c11;
 t2 = dsa.y * dsb.y + ((c2 - e) + (c11 - (t1 - e))) + c21;
 
 dsc.x = t1 + t2;
 dsc.y = t2 - (dsc.x - t1);
 
 return dsc;
}

// create double-single number from float
vec2 ds_set(float a)
{
 vec2 z;
 z.x = a;
 z.y = 0.0;
 return z;
}

/////////////////
// Main Shader //
/////////////////
void main()
{
  vec2 xCo = ds_mul(ds_mul(ds_set(gl_FragCoord.x),ds_set(Zoom)),ds_set(1.0/960.0));
  vec2 yCo = ds_mul(ds_mul(ds_set(gl_FragCoord.y-120),ds_set(Zoom)),ds_set(1.0/960.0));

  if (Julia)
  {
    // Adjust for the julia's position
    xCo = ds_mul(ds_mul(ds_set(gl_FragCoord.x-960),ds_set(Zoom)),ds_set(1.0/960.0));
  }

  vec2 real = ds_sub(ds_sub(xCo, ds_set(Zoom/2.0)), ds_set(Xcenter));
  vec2 imag = ds_sub(ds_sub(yCo, ds_set(Zoom/2.0)), ds_set(Ycenter));

  vec2 Creal = real;
  vec2 Cimag = imag;

  if (Julia)
  {
    Creal = ds_set(JuliaA);
    Cimag = ds_set(JuliaB);
  }

  vec2 r2 = ds_set(0.0);

  // Useful constants
  vec2 negOne = ds_set(-1.0);
  vec2 one = ds_set(1.0);
  vec2 ptOne = ds_set(0.1);
  vec2 two = ds_set(2.0);

  float iter;
  vec2 tempreal;

  vec2 radius = ds_set(4.0);

  for (iter = 0.0; iter < MaxIterations; ++iter)
  {
    tempreal = real;
    real = ds_add(ds_sub(ds_mul(tempreal, tempreal), ds_mul(imag, imag)), Creal);
    imag = ds_add(ds_mul(ds_mul(imag, tempreal), two), Cimag);

    if (Almond)
    {
      tempreal = real;

      real = ds_mul(negOne, ds_sub(imag, ds_mul(ptOne, real)));
      imag = ds_add(ds_add(one, tempreal), imag);
    }

    r2 = ds_add(ds_mul(real, real), ds_mul(imag, imag));
    if (ds_compare(r2, radius) > 0.0)
      break;
  }


  // Base the color on the number of iterations
  float color;
  
  if (ds_compare(r2, ds_set(4.0)) < 0.0)
    color = 0.0;
  else
  {
    if (LogShading)
      color = (float(iter) + 1. - log(log(length(vec2(real.x,imag.x))))/log(2.));  // http://linas.org/art-gallery/escape/escape.htm
    else
      color = iter;
  }
  
  FragColor = vec4((-cos(R*0.25*color)+1.0)/2.0, 
            (-cos(B*0.25*color)+1.0)/2.0, 
            (-cos(G*0.25*color)+1.0)/2.0,
               1.0);
}
