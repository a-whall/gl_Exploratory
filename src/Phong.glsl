#shader vertex
#version 410

layout (location = 0) in vec3 vin_pos;
layout (location = 1) in vec3 vin_norm;
 
out vec3 vout_color;
 
uniform struct LightIntensity {
  vec4 camCoordPos;
  vec3 ambi;
  vec3 diff;
  vec3 spec;
} light;
 
uniform struct MaterialReflectivity {
  vec3 ambiRefl;
  vec3 diffRefl;
  vec3 specRefl;
  float sheen;  //shininess factor
} mater;
 
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
 
void main()
{
  vec3 n = normalize( NormalMatrix * vin_norm);
  vec4 vPos_CamCoords = ModelViewMatrix * vec4(vin_pos, 1.0);         // vertex position in view space (camera as origin)
    
  vec3 ambient = light.ambi * mater.ambiRefl;
  vec3 s = normalize(vec3(light.camCoordPos - vPos_CamCoords));    // direction of incoming light
  float sDotN = max( dot(s,n), 0.0 );                             // dot outputs [-1,1], max reduces range to [0,1]
  vec3 diffuse = light.diff * mater.diffRefl * sDotN;
  vec3 spec = vec3(0.0);
  if( sDotN > 0.0 ) {
    vec3 v = normalize(-vPos_CamCoords.xyz);                  // direction to camera position
    vec3 r = reflect( -s, n );                               // reflection of light hitting the object about the vertex normal or surface normal
    spec = light.spec * mater.specRefl *  pow( max( dot(r,v), 0.0 ), mater.sheen );
  }

  vout_color = ambient + diffuse + spec;
  gl_Position = MVP * vec4(vin_pos, 1.0);
}

#shader fragment
#version 410

in vec3 vout_color;
layout( location = 0 ) out vec4 fout_color;

void main() {
    fout_color = vec4(vout_color, 1.0);
}
#end