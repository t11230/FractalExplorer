#version 130
/////////////////////////////////////
// This is where the magic happens //
/////////////////////////////////////

// Fractal Parameters
uniform float MaxIterations;
uniform float Zoom;
uniform float Xcenter;
uniform float Ycenter;
uniform float JuliaA;
uniform float JuliaB;

// Coloring coefficients
uniform float R;
uniform float G;
uniform float B;

// Interface Parameters
uniform bool Almond;
uniform bool LogShading;
uniform bool Julia;

// Color that pixel
out vec4 FragColor;

void main()
{
  // Convert our coordinate in fragment shader XY plane to
  // coordinates in the fractal's coordinate system
  // Props to Aaron for deriving this equation.
  float real = (gl_FragCoord.x*Zoom)/960.0 - Zoom/2.0 - Xcenter;
  float imag = ((gl_FragCoord.y-120)*Zoom)/960.0 - Zoom/2.0 - Ycenter;

  // Initialize the C values for the mandelbrot
  float Creal = real;
  float Cimag = imag;

  // If this is tha Julia set, adjust C accordingly
  if (Julia)
  {
    // We need to change the coordinate coversion because the
    // view port is located differently for the Julia
    // There is a better way, but I could not find anyone who had
    // done it with SFML
    real = ((gl_FragCoord.x-960)*Zoom)/960.0 - Zoom/2.0 - Xcenter;
    imag = ((gl_FragCoord.y-120)*Zoom)/960.0 - Zoom/2.0 - Ycenter;

    // Set out C for the Julia set
    Creal = JuliaA;
    Cimag = JuliaB;
  }

  // r2 holds the current length squared
  float r2 = 0.0;
  // Keep track of our iteration count
  float iter;

  // Iterate!
  for (iter = 0.0; iter < MaxIterations && r2 < 4.0; ++iter)
  {
    // Standard maths
    float tempreal = real;
    real = (tempreal * tempreal) - (imag * imag) + Creal;
    imag = 2.0 * tempreal * imag + Cimag;

    // If we are doing the almond bread thing
    if (Almond)
    {
      tempreal = real;

      // Below are different coordinate transforms that I tried

      // Bi-linear
      // real = -1.*(imag-0.1*real);
      // imag = 1.+tempreal;
      
      // Log-polar coordinates
      // real = log(length(vec2(real,imag)));
      // imag = atan(imag, tempreal);

      // Bi-polar coordinates
      // real = sinh(imag)/(cosh(imag) - cos(real));
      // imag = sin(real)/(cosh(imag) - cos(real));

      // I think this one is pretty cool
      real = -1.*(imag-0.1*real);
      imag = 1.+tempreal+imag;
    }

    // Update the length of the current vector
    r2 = (real * real) + (imag * imag);
  }

  // Base the color on the number of iterations
  float color;
  
  // Black if we dont escape
  if (r2 < 4.0)
    color = 0.0;
  else
  {
    if (LogShading)
      color = (float(iter) + 1. - log(log(length(r2)))/log(2.));
      // http://linas.org/art-gallery/escape/escape.htm
    else
      color = iter;
  }
  
  FragColor = vec4((-cos(R*0.25*color)+1.0)/2.0, 
            (-cos(B*0.25*color)+1.0)/2.0, 
            (-cos(G*0.25*color)+1.0)/2.0, 
               1.0);

}
