#   Blender Mesh Export for the Whipstitch Game Engine
#   Copyright D. Scott Nettleton, 2013
#   This software is released under the terms of the
#   Lesser GNU Public License (LGPL).
import bpy,os
from math import sqrt
from mathutils import Matrix, Vector, Quaternion

workingDirectory = os.path.split(bpy.data.filepath)[0] + "/"
MAJOR_VERSION = 0
MINOR_VERSION = 8
BLENDER_FPS = 24

class wsJointMod:
  def __init__(self, name):
    self.name = name
    self.jointIndex = 0
    self.location = Vector((0,0,0))
    self.rotation = Quaternion((1,0,0,0))
    self.initialRot = Quaternion((1,0,0,0))
    #end jointModification constructor
  def swap(self, curveIndex, value):
    if curveIndex == 0:
      # Blender Location X value
      self.location.x = value
    elif curveIndex == 1:
      # Blender Location Y value
      self.location.z = -value
    elif curveIndex == 2:
      # Blender Location Z value
      self.location.y = value
    elif curveIndex == 3:
      # Blender Rotation W value
      self.rotation.w = value
    elif curveIndex == 4:
      # Blender Rotation X value
      self.rotation.x = value
    elif curveIndex == 5:
      # Blender Rotation Y value
      self.rotation.z = -value
    elif curveIndex == 6:
      # Blender Rotation Z value
      self.rotation.y = value
  #end class jointModification

class wsJoint:
  def __init__(self, name):
    self.name = name
    self.start = Vector((0,0,0))
    self.end = Vector((0,0,0))
    self.rot = Quaternion((1,0,0,0))
    self.initialRot = Quaternion((1,0,0,0))
    self.parent = -1

class wsSkeleton:
  def __init__(self, name):
    self.name = name
    self.numJoints = 0
    self.joints = []
    self.location = Vector((0,0,0))
    self.rotation = Quaternion((1,0,0,0))
    self.scale = Vector((1,1,1))

class wsKeyframe:
  def __init__(self, index):
    self.frameIndex = index
    self.numJointMods = 0
    self.jointMods = []

class wsAnimation:
  def __init__(self, name, skeleton):
    self.name = name
    self.numKeyframes = 0
    self.keyframes = []
    self.skeleton = skeleton
    self.framesPerSecond = BLENDER_FPS
    self.length = 0.0

class wsMesh:
  def __init__(self, name, skeleton):
    self.name = name
    self.numVerts = 0
    self.numMaterials = 0
    self.numTags = 0
    self.verts = []
    self.materials = []
    self.tags = []
    self.skeleton = skeleton
    self.location = Vector((0,0,0))
    self.scale = Vector((1,1,1))
    self.rotation = Quaternion((1,0,0,0))

class wsVert:
  def __init__(self):
    self.pos = Vector((0,0,0))
    self.norm = Vector((1,0,0))
    self.texCoords = [0, 0]
    self.numWeights = 0
    self.weights = []
    self.weightSum = 0

class wsWeight:
  def __init__(self, index, influence):
    self.jointIndex = index
    self.influence = influence

class wsTri:
  def __init__(self, val1, val2, val3):
    self.vertIndices = [val1, val2, val3]

class wsMaterial:
  def __init__(self, name):
    self.name = name
    self.ambient = [0, 0, 0, 1]
    self.diffuse = [0, 0, 0, 1]
    self.specular = [0, 0, 0, 1]
    self.emissive = [0, 0, 0, 1]
    self.numTris = 0
    self.tris = []
    self.shininess = 0
    self.colorMap = ""

class wsTag:
  def __init__(self, name):
    self.name = name
    self.pos = Vector((0,0,0))
    self.rot = Quaternion((1,0,0,0))

class wsFileBuffer:
  def __init__(self, filename, ext):
    self.filename = filename +"."+ ext
    self.buffer = open(workingDirectory + self.filename, "w")
  def write(self, text):
    self.buffer.write(text)
  def apply(self):
    self.buffer.close()
  def clear(self):
    self.buffer = ""

numTags = 0
animSkel = None
animSkelName = ""
mesh = None
animations = []
tags  = []

#   Calculate Skeletal and Tag data
for my in bpy.data.objects:
  if my.type == "EMPTY":
    tag = wsTag(my.name)
    tag.pos.x = my.location[0]
    tag.pos.y = my.location[2]
    tag.pos.z = -my.location[1]
    tag.rot.x = my.rotation_quaternion[1]
    tag.rot.y = my.rotation_quaternion[3]
    tag.rot.z = -my.rotation_quaternion[2]
    tag.rot.w = my.rotation_quaternion[0]
    tags.append( tag )
    #end if type == "EMPTY"
  elif my.type == "ARMATURE":
    animSkel =  wsSkeleton(my.name)
    animSkelName = my.name
    jointNum = 0
    joints = []
    for boneName in my.data.bones.keys():
      bone = my.data.bones[boneName]
      joint = wsJoint(boneName)
      joint.start.x = bone.head_local[0]
      joint.start.y = bone.head_local[2]
      joint.start.z = -bone.head_local[1]
      joint.end.x = bone.tail_local[0]
      joint.end.y = bone.tail_local[2]
      joint.end.z = -bone.tail_local[1]
      joint.rot = Quaternion(bone.matrix_local.to_quaternion())
      tmpZ = joint.rot.z
      joint.rot.z = -joint.rot.y
      joint.rot.y = tmpZ
      parentNum = 0
      for bone2 in my.data.bones:
        if bone2 == bone.parent:
          joint.parent = parentNum
          break
        parentNum += 1
        # End for each bone2
      joints.append( joint )
      jointNum += 1
      #end for each boneName
    animSkel.joints = joints
    animSkel.numJoints = jointNum
    animSkel.location = Vector([ my.location[0], my.location[2], -my.location[1] ])
    animSkel.rotation.x = my.rotation_quaternion[1]
    animSkel.rotation.y = my.rotation_quaternion[3]
    animSkel.rotation.z = -my.rotation_quaternion[2]
    animSkel.rotation.w = my.rotation_quaternion[0]
    animSkel.scale = Vector([ my.scale[0], my.scale[2], my.scale[1] ])
    #end if type == "ARMATURE"
  #end for each object

#   Calculate Animation Data
for my in bpy.data.actions:
  anim = wsAnimation(my.name, animSkel)
  keyFrames = []
  for curve in my.fcurves:
    for kPoint in curve.keyframe_points:
      inList = 0
      for key in keyFrames:
        if key == kPoint.co.x:
          inList = 1
          break
        #end for each key in keyFrames
      if inList == 0:
        keyFrames.append(kPoint.co.x)
  anim.numKeyframes = len(keyFrames)
  anim.length = keyFrames[len(keyFrames)-1] / BLENDER_FPS
  for index in keyFrames:
    key = wsKeyframe(index)
    key.jointMods = [None]*len(animSkel.joints)
    for group in my.groups:
      modified = 0
      mod = wsJointMod(group.name)
      for j in range( len(animSkel.joints) ):
        if animSkel.joints[j].name == group.name:
          mod.jointIndex = j
          break

      # mod.jointIndex = index
      curveIndex = 0
      for curve in group.channels:
        for kPoint in curve.keyframe_points:
          if kPoint.co.x == index:
            modified = 1
            mod.swap(curveIndex, kPoint.co.y)
          #end for each keyframe_point
        curveIndex += 1
        #end for each fcurve
      if modified == 1:
        # key.jointMods.append(mod)
        key.jointMods[mod.jointIndex] = mod
      #end for each action group
    key.numJointMods = len(key.jointMods)
    anim.keyframes.append(key)
    #end for each keyframe
  animations.append(anim)
  #end for each animation

#   Calculate Mesh Data
for my in bpy.data.objects:
  if my.type == "MESH":
    my.data.calc_tessface()
    mesh = wsMesh(my.name, animSkel)
    location = my.matrix_world.to_translation()
    scale = my.matrix_world.to_scale()
    rotation = my.matrix_world.to_quaternion()
    mesh.location = Vector([ location[0], location[2], -location[1] ])
    mesh.scale = Vector([ scale.x, scale.z, scale.y ])
    mesh.rotation.x = rotation[1]
    mesh.rotation.y = rotation[3]
    mesh.rotation.z = -rotation[2]
    mesh.rotation.w = rotation[0]
    mesh.tags = tags
    mesh.numTags = len(tags)
    matCount = 0
    vertIndexCount = 0
    for myMat in my.data.materials:
      faceNum = 0
      mat = wsMaterial(myMat.name)
      for face in my.data.tessfaces:
        if face.material_index == matCount:
          for v in range( len(face.vertices) ):
            vert = wsVert()
            bVert = my.data.vertices[face.vertices[v]]
            vert.pos = Vector([ bVert.co[0], bVert.co[2], -bVert.co[1] ])
            vert.norm = Vector([ bVert.normal[0], bVert.normal[2], -bVert.normal[1] ])
            if v == 0:
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv1[0], \
                                              my.data.tessface_uv_textures[0].data[faceNum].uv1[1] ]
            elif v == 1:
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv2[0], \
                                              my.data.tessface_uv_textures[0].data[faceNum].uv2[1] ]
            elif v == 2:
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv3[0], \
                                              my.data.tessface_uv_textures[0].data[faceNum].uv3[1] ]
            elif v == 3:
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv4[0], \
                                              my.data.tessface_uv_textures[0].data[faceNum].uv4[1] ]
            for g in range(len(bVert.groups)):
              vertGroup = my.vertex_groups[bVert.groups[g].group]
              jointid = -1
              boneCount = 0
              for thisBone in animSkel.joints:
                if thisBone.name == vertGroup.name:
                  jointid = boneCount
                  break
                boneCount += 1
              vert.weights.append( wsWeight(jointid, bVert.groups[g].weight) )
              vert.numWeights += 1
              vert.weightSum += bVert.groups[g].weight
              #end for each vertex group
            mesh.verts.append(vert)
            #end for each vertex
          mat.tris.append( wsTri(vertIndexCount, vertIndexCount+1, vertIndexCount+2) )
          if len(face.vertices) == 4:
            mat.tris.append( wsTri(vertIndexCount, vertIndexCount+2, vertIndexCount+3) )
            vertIndexCount += 1
          vertIndexCount += 3
          #end if this face uses the current material
        faceNum += 1
        #end for each face

      mat.numTris = len(mat.tris)
      mat.ambient = [ myMat.ambient * myMat.diffuse_color[0], \
                                  myMat.ambient * myMat.diffuse_color[1], \
                                  myMat.ambient * myMat.diffuse_color[2], \
                                  myMat.ambient * myMat.alpha ]
      mat.diffuse = [ myMat.diffuse_color[0], \
                                  myMat.diffuse_color[1], \
                                  myMat.diffuse_color[2], \
                                  myMat.alpha ]
      mat.specular = [ myMat.specular_color[0], \
                                  myMat.specular_color[1], \
                                  myMat.specular_color[2], \
                                  myMat.specular_alpha ]
      mat.emissive = [ myMat.emit * myMat.diffuse_color[0], \
                                  myMat.emit * myMat.diffuse_color[1], \
                                  myMat.emit * myMat.diffuse_color[2], \
                                  myMat.emit * myMat.alpha ]
      mat.shininess = myMat.specular_hardness
      mat.colorMap = myMat.active_texture.image.filepath
      mesh.materials.append( mat )
      matCount += 1
      #end for each material
      mesh.numMaterials = len(mesh.materials)
      mesh.numVerts = len(mesh.verts)
    #end for each mesh object

#   ADJUST DATA FOR PARENT OBJECT TRANSFORMATIONS
if animSkel != None:
  animSkel.numJoints = len(animSkel.joints)
  for joint in animSkel.joints:
    joint.start.rotate(animSkel.rotation)
    joint.end.rotate(animSkel.rotation)
    joint.start.x *= animSkel.scale.x
    joint.start.y *= animSkel.scale.y
    joint.start.z *= animSkel.scale.z
    joint.end.x *= animSkel.scale.x
    joint.end.y *= animSkel.scale.y
    joint.end.z *= animSkel.scale.z
    joint.start += animSkel.location
    joint.end += animSkel.location
    joint.initialRot = joint.rot
    joint.rot = animSkel.rotation * joint.initialRot
if mesh != None:
  mesh.numVerts = len(mesh.verts)
  mesh.numTags = len(mesh.tags)
  mesh.numMaterials = len(mesh.materials)
  for mat in mesh.materials:
    mat.numTris = len(mat.tris)
  for vert in mesh.verts:
    vert.pos.x *= mesh.scale.x
    vert.pos.y *= mesh.scale.y
    vert.pos.z *= mesh.scale.z
    vert.pos += mesh.location
    vert.pos.rotate(mesh.rotation)
    vert.numWeights = len(vert.weights)
for anim in animations:
  anim.numKeyframes = len(anim.keyframes)
  for key in anim.keyframes:
    key.numJointMods = len(key.jointMods)
    for j in range( key.numJointMods ):
      boneMat = None
      skel = bpy.data.objects[animSkelName]
      key.jointMods[j].location.x *= animSkel.scale.x
      key.jointMods[j].location.y *= animSkel.scale.y
      key.jointMods[j].location.z *= animSkel.scale.z
      #key.jointMods[j].location.rotate(animSkel.rotation)
      key.jointMods[j].initialRot = key.jointMods[j].rotation
      key.jointMods[j].rotation = animSkel.rotation * animSkel.joints[j].initialRot * key.jointMods[j].initialRot
      key.jointMods[j].location.rotate(key.jointMods[j].rotation)
      par = animSkel.joints[j].parent
      if par >= 0:
        diffRot = animSkel.joints[par].initialRot.inverted() * animSkel.joints[j].initialRot
        key.jointMods[j].rotation = key.jointMods[par].rotation * diffRot * key.jointMods[j].initialRot
        

#   NOW WE WRITE!
if mesh != None:
  output = wsFileBuffer(mesh.name, "wsMesh")
  output.write("//  Whipstitch Mesh File\n")
  output.write("//  This mesh is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("meshName "+ mesh.name +"\n")
  output.write("numVertices "+ str(mesh.numVerts) +"\n")
  output.write("numMaterials "+ str(mesh.numMaterials) +"\n")
  output.write("numTags "+ str(mesh.numTags) +"\n\n")
  output.write("skeleton {\n")
  skel = mesh.skeleton
  output.write("  numJoints "+ str(skel.numJoints) +"\n") #add one for the root location
  for j in range( skel.numJoints ):
    output.write("  joint "+ str(j) +" {\n")
    output.write("    parent "+ str(skel.joints[j].parent) +"\n")
    output.write("    pos_start { %f %f %f }\n" % (skel.joints[j].start.x, skel.joints[j].start.y, skel.joints[j].start.z))
    output.write("    pos_end { %f %f %f }\n" % (skel.joints[j].end.x, skel.joints[j].end.y, skel.joints[j].end.z))
    output.write("    rotation { %f %f %f %f }\n" % (skel.joints[j].rot.x, skel.joints[j].rot.y, skel.joints[j].rot.z, \
      skel.joints[j].rot.w))
    output.write("  }\n")
    #end for each joint
  output.write("}\n\n")
  output.write("vertices {\n")
  for v in range( mesh.numVerts ):
    vert = mesh.verts[v]
    output.write("  vert "+ str(v) +" {\n")
    output.write("    pos { %f %f %f }\n    norm { %f %f %f }\n    tex { %f %f }\n" % \
      (mesh.verts[v].pos.x, mesh.verts[v].pos.y, mesh.verts[v].pos.z, \
      mesh.verts[v].norm.x, mesh.verts[v].norm.y, mesh.verts[v].norm.z, \
      mesh.verts[v].texCoords[0], mesh.verts[v].texCoords[1]) )
    output.write("    weights {\n")
    output.write("      numWeights "+ str(mesh.verts[v].numWeights) +"\n")
    for w in range( mesh.verts[v].numWeights):
      weightVal = mesh.verts[v].weights[w].influence
      if mesh.verts[v].weightSum != 0:
        weightVal /= mesh.verts[v].weightSum
      output.write("      joint { %d %f }\n" % \
        (mesh.verts[v].weights[w].jointIndex, weightVal))
      #end for each weight
    output.write("    }\n")
    output.write("  }\n")
    #end for each vertex
  output.write("}\n\n")
  output.write("materials {\n")
  for m in range( mesh.numMaterials ):
    mat = mesh.materials[m]
    output.write("  mat "+ str(m) +" {\n")
    output.write("    name "+ mat.name +"\n")
    output.write("    shine "+ str(mat.shininess) +"\n")
    output.write("    ambient {"+ \
      str(mat.ambient[0]) +" "+ \
      str(mat.ambient[1]) +" "+ \
      str(mat.ambient[2]) +" "+ \
      str(mat.ambient[3]) +" }\n")
    output.write("    diffuse {"+ \
      str(mat.diffuse[0]) +" "+ \
      str(mat.diffuse[1]) +" "+ \
      str(mat.diffuse[2]) +" "+ \
      str(mat.diffuse[3]) +" }\n")
    output.write("    specular {"+ \
      str(mat.specular[0]) +" "+ \
      str(mat.specular[1]) +" "+ \
      str(mat.specular[2]) +" "+ \
      str(mat.specular[3]) +" }\n")
    output.write("    emissive {"+ \
      str(mat.emissive[0]) +" "+ \
      str(mat.emissive[1]) +" "+ \
      str(mat.emissive[2]) +" "+ \
      str(mat.emissive[3]) +" }\n")
    output.write("    maps {\n")
    output.write("      colorMap "+ mat.colorMap +"\n")
    output.write("    }\n")
    output.write("    numTriangles "+ str(mat.numTris) +"\n")
    output.write("    triangles {\n")
    for t in range(mat.numTris):
      output.write("      tri "+ str(t) +" {\n")
      output.write("        verts {\n")
      output.write("          indices { "+ \
        str(mat.tris[t].vertIndices[0]) +" "+ \
        str(mat.tris[t].vertIndices[1]) +" "+ \
        str(mat.tris[t].vertIndices[2]) +" }\n")
      output.write("        }\n")
      output.write("      }\n")
      #end for each triangle
    output.write("    }\n")
    output.write("  }\n")
    #end for each material
  output.write("}\n\n")
  output.write("tags {\n")
  for t in range(mesh.numTags):
    output.write("  tag "+ str(t) +" {\n")
    output.write("    name "+ mesh.tags[t].name +"\n")
    output.write("    pos { "+ \
      str(mesh.tags[t].pos.x) +" "+ \
      str(mesh.tags[t].pos.y) +" "+ \
      str(mesh.tags[t].pos.z) +" }\n")
    output.write("    dir { "+ \
      str(mesh.tags[t].rot.x) +" "+ \
      str(mesh.tags[t].rot.y)+" "+ \
      str(mesh.tags[t].rot.z) +" "+ \
      str(mesh.tags[t].rot.w) +" }\n")
    output.write("  }\n")
    #end for each tag
  output.write("}\n\n")
  output.apply()
  #end if mesh exists

for a in range( len(animations) ):
  anim = animations[a]
  skel = anim.skeleton
  output = wsFileBuffer(anim.name, "wsAnim")
  output.write("//  Whipstitch Animation File\n")
  output.write("//  This Animation is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("animationType 1\n")
  output.write("animationName "+ anim.name +"\n")
  output.write("framesPerSecond "+ str(anim.framesPerSecond) +"\n\n")
  output.write("numJoints "+str(skel.numJoints)+"\n") #add one for the root location
  output.write("numKeyFrames "+ str(anim.numKeyframes) +"\n\n")
  output.write("//  Animation files store a collection of joint rotations; matrices are computed on loading\n\n")
  output.write("joints {\n")
  for j in range( skel.numJoints ):
    joint = skel.joints[j]
    output.write("  joint "+ str(j) +" {\n")
    output.write("    jointName "+ joint.name +"\n")
    output.write("    parent "+ str(joint.parent) +"\n")
    output.write("    pos_start { "+ \
      str(joint.start.x) +" "+ \
      str(joint.start.y) +" "+ \
      str(joint.start.z) +" }\n")
    output.write("    rotation { "+ \
      str(joint.rot.x) +" "+ \
      str(joint.rot.y) +" "+ \
      str(joint.rot.z) +" "+ \
      str(joint.rot.w) +" }\n")
    output.write("  }\n")
    #end for each joint
  output.write("}\n\n")
  output.write("keyframes {\n")
  for k in range( anim.numKeyframes ):
    key = anim.keyframes[k]
    output.write("  keyframe "+ str(k) +" {\n")
    output.write("    frameNumber "+ str(key.frameIndex) +"\n")
    output.write("    jointsModified "+ str(key.numJointMods) + "\n")
    for j in range( key.numJointMods ):
      output.write("    joint "+ str(j) +" {\n")
      # output.write("      jointName "+ key.jointMods[j].name +"\n")
      output.write("      jointTranslation { %f %f %f }\n" % (key.jointMods[j].location.x, key.jointMods[j].location.y, \
        key.jointMods[j].location.z))
      output.write("      jointRotation { %f %f %f %f }\n" % (key.jointMods[j].rotation.x, key.jointMods[j].rotation.y, \
        key.jointMods[j].rotation.z, key.jointMods[j].rotation.w))
      output.write("    }\n")
      #end for each joint mod
    output.write("  }\n")
    #end for each keyframe
  output.write("}\n")
  output.apply()
  #end for each animation


if mesh != None:
  output = wsFileBuffer(mesh.name, "wsModel")
  output.write("//  Whipstitch Model File\n")
  output.write("//  This model is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("modelName "+ mesh.name +"\n\n")
  output.write("numMeshes 1\n")
  output.write("numAnimations %d\n" % (len(animations)))

print("Export Complete.")
