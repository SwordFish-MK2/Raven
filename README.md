# Raven Introduction

![image](https://github.com/FrenchFries-With-Pepper/Raven/blob/main/results/cornellbox/cornellbox_path_tracing.jpg)

Raven is an under developing offline ray tracing renderer written in C++17 and runs on windows. It provides basic implementation of the functionalities required to solve the rendering equation. 

Raven only supports reflective materials by now, transmissive materials and volume scattering participating media will be  added in the future. 

## News

- 2022/4/16 Glass and Mirror material is  supported.  Raven has added two classes that handle specular reflection and specular transmission respectively.
- 2022/4/2	Microfacet shading models is now available! Raven is now supporting  smith GGX and Beckmann microfacet distributions, it also adds a new class to compute fresnel value for dielectrics and conductors.  																																					

## Key modules

| Name                 | Description                                                  |
| :------------------- | :----------------------------------------------------------- |
| Renderer(Integrator) | Loop over film, solve rendering equation and compute pixel color |
| Film                 | Store pixel value computed by renderer class, filter and output rendered image |
| Camera               | Generate rays                                                |
| Light Source         | Provide illumination to scene                                |
| Scene                | Store shapes,lights and their related pointers, passing them to renderer |
| Shape                | Geometric entities in the scene                              |
| Material             | Surface attributes of a primitive, composed of a set of shading models |
| Shading model(BxDF)  | Implementation of surface bxdf, decide reflection/ transmission direction and energy attenuation |
| Texture              | a set of values mapped to 2D surface/3D volume               |
| Sampler              | Generating random numbers between [0,1]                      |



## Implements 

### Integrators

- Path Tracing

- Direct Lighting

### Cameras

- Perspective Camera
- Orthographic Camera

### Light Sources

- Diffuse Area Lights

### Shapes

- Sphere
- Triangle Meshes

### Shading Models

- Lambertain
- Smith GGX
- Beckmann-Spizzichino
- Fresnel reflection(Specular reflection)
- Fresnel transmission(Specular transmission)

### Materials

- Mirror(Undebuged)
- Glass 
- Matte
- Plastic(Undebuged)

### Textures

- Constant Texture
- Checkered Texture
- Image Texture (Under developing)

