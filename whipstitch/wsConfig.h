/*
 *  wsConfig.h
 */

#ifndef WS_CONFIG_
#define WS_CONFIG_

#define WS_UDP_PORT 42042

#define WS_NUM_COLLISION_CLASSES 5
#define WS_NUM_FBO_TEX 8
#define WS_NUM_FRAMEBUFFERS 2
#define WS_NUM_SHADERS 4

#define WS_MAX_CAMERAS 16
#define WS_MAX_FRAME_SKIPS 4
#define WS_MAX_JOINT_INFLUENCES 4
#define WS_MAX_JOINTS 128
#define WS_MAX_MODELS 128
#define WS_MAX_PRIM_MATERIAL_PROPERTIES 8
#define WS_MAX_PRIMITIVES 256
#define WS_MAX_TEXTURES 32

#define WS_DEFAULT_FPS 60
#define WS_DEFAULT_MAX_ANIMATIONS 32

//  Shapes
enum {
  WS_SHAPE_SPHERE,
  WS_SHAPE_CYLINDER,
  WS_SHAPE_CUBE,
  WS_SHAPE_CAPSULE
};// End enum Shapes

//  Mesh Formats
enum {
  WS_MESH_FORMAT_WHIPSTITCH,
  WS_MESH_FORMAT_STL
};// End enum Mesh Formats

enum {
  WS_SHADER_INITIAL,
  WS_SHADER_FINAL,
  WS_SHADER_POST,
  WS_SHADER_HUD,
  WS_SHADER_OUTLINE,
  WS_SHADER_ANTIALIAS,
  WS_SHADER_DEBUG
};//  End enum Shaders

enum {
  WS_FBO_TEX_POS,
  WS_FBO_TEX_NORM,
  WS_FBO_TEX_TEXTURE,
  WS_FBO_TEX_MAT,
  WS_FBO_TEX_DEPTH,
  WS_FBO_TEX_FINAL_A,
  WS_FBO_TEX_FINAL_B
};//  End fbo textures

enum {
  WS_FBO_PRIMARY,
  WS_FBO_POST_A,
  WS_FBO_POST_B
};//  End framebuffers

enum {
  WS_VERT_ATTRIB_POSITION,
  WS_VERT_ATTRIB_NORMAL,
  WS_VERT_ATTRIB_TEX_COORDS,
  WS_VERT_ATTRIB_NUM_WEIGHTS,
  WS_VERT_ATTRIB_JOINT_INDEX,
  WS_VERT_ATTRIB_INFLUENCE,
  WS_VERT_ATTRIB_JOINT_INDEX_2,
  WS_VERT_ATTRIB_INFLUENCE_2
};//  End Vertex Attributes

enum {
  WS_DRAW_BONES     = 0x0001,
  WS_DRAW_CEL       = 0x0002,
  WS_DRAW_BOUNDS    = 0x0004,
  WS_DRAW_NORM_MAPS = 0x0008,
  WS_DRAW_OUTLINE   = 0x0010,
  WS_DRAW_ANTIALIAS = 0x0020,
  WS_DRAW_CULL_FACE = 0x0040,
  WS_DRAW_LIGHTING  = 0x0080,
  WS_DRAW_DEPTH     = 0x0100,
  WS_DRAW_CURSOR    = 0x0200,
  WS_DRAW_TEXTURES  = 0x0400,
  WS_DRAW_AXES      = 0x0800
};//  End Drawing features

enum {
  WS_MODEL_LOCK_HORIZ_ROTATIONS = 0x0001,
  WS_MODEL_ATTACHED             = 0x0002,
  WS_MODEL_ANIM_PAUSED          = 0x0004
};//  End Model Properties

enum {
  WS_TEXTURE_MAP_COLOR =  0x0001,
  WS_TEXTURE_MAP_NORMAL = 0x0002
};//  End texture map bitflags

#endif