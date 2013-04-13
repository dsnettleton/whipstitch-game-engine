#   Blender Mesh Export for the Whipstitch Game Engine
#   Copyright D. Scott Nettleton, 2013
#   This software is released under the terms of the
#   Lesser GNU Public License (LGPL).
import bpy,os
from math import sqrt
from mathutils import Matrix, Vector, Quaternion
from copy import deepcopy

workingDirectory = "./"
MAJOR_VERSION = 1
MINOR_VERSION = 2
BLENDER_FPS = 24

WS_TEXTURE_MAP_COLOR  = 0x0001
WS_TEXTURE_MAP_NORMAL = 0x0002

class wsJointMod:
  def __init__(self, name):
    self.name = name
    self.jointIndex = 0
    self.location = Vector((0,0,0))
    self.rotation = Quaternion((1,0,0,0))
    self.initialRot = Quaternion((1,0,0,0))
    #end jointModification constructor
  def swap(self, curveIndex, value):
    if (curveIndex == 0):
      # Blender Location X value
      self.location.x = value
    elif (curveIndex == 1):
      # Blender Location Y value
      self.location.z = -value
    elif (curveIndex == 2):
      # Blender Location Z value
      self.location.y = value
    elif (curveIndex == 3):
      # Blender Rotation W value
      self.rotation.w = value
    elif (curveIndex == 4):
      # Blender Rotation X value
      self.rotation.x = value
    elif (curveIndex == 5):
      # Blender Rotation Y value
      self.rotation.z = -value
    elif (curveIndex == 6):
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
    self.bounds = wsBounds(0,0,0,0,0,0)

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
    self.bounds = wsBounds(0,0,0,0,0,0)

class wsAnimation:
  def __init__(self, name, skeleton):
    self.name = name
    self.numKeyframes = 0
    self.keyframes = []
    self.skeleton = skeleton
    self.framesPerSecond = BLENDER_FPS
    self.length = 0.0
    self.bounds = wsBounds(0,0,0,0,0,0)

class wsBounds:
  def __init__(self, minX, maxX, minY, maxY, minZ, maxZ):
    self.minX = minX
    self.maxX = maxX
    self.minY = minY
    self.maxY = maxY
    self.minZ = minZ
    self.maxZ = maxZ
    self.halfX = 0.0
    self.halfY = 0.0
    self.halfZ = 0.0

class wsMesh:
  def __init__(self, name, skeleton):
    self.name = name
    self.numVerts = 0
    self.numMaterials = 0
    self.verts = []
    self.materials = []
    self.skeleton = skeleton
    self.location = Vector((0,0,0))
    self.scale = Vector((1,1,1))
    self.rotation = Quaternion((1,0,0,0))
    self.bounds = wsBounds(0,0,0,0,0,0)

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
    self.mapBitFlag = 0
    self.colorMap = ""
    self.normalMap = ""
    self.properties = []

class wsCustomProperty:
  def __init__(self, name, value):
    self.name = name
    self.value = value

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

animSkel = None
animSkelName = ""
mesh = None
animations = []
hasSkeleton = 0
boundsInitialized = 0
bpy.ops.object.mode_set(mode='OBJECT')  # Make sure we're in object mode

#   Calculate Skeletal data
for my in bpy.data.objects:
  if (my.type == "ARMATURE"):
    hasSkeleton = 1
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
        if (bone2 == bone.parent):
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
    #end if (type == "ARMATURE")
  #end for each object

#   Calculate Animation Data
for my in bpy.data.actions:
  if (my.id_root == 'OBJECT'):
    anim = wsAnimation(my.name, animSkel)
    keyFrames = []
    for curve in my.fcurves:
      for kPoint in curve.keyframe_points:
        inList = 0
        for key in keyFrames:
          if (key == kPoint.co.x):
            inList = 1
            break
          #end for each key in keyFrames
        if (inList == 0):
          keyFrames.append(kPoint.co.x)
    anim.numKeyframes = len(keyFrames)
    if (anim.numKeyframes <= 1):
      continue
    anim.length = keyFrames[len(keyFrames)-1] / BLENDER_FPS
    for index in keyFrames:
      key = wsKeyframe(index)
      key.jointMods = [None]*len(animSkel.joints)
      for group in my.groups:
        modified = 0
        mod = wsJointMod(group.name)
        for j in range( len(animSkel.joints) ):
          if (animSkel.joints[j].name == group.name):
            mod.jointIndex = j
            break

        # mod.jointIndex = index
        curveIndex = 0
        for curve in group.channels:
          for kPoint in curve.keyframe_points:
            if (kPoint.co.x == index):
              modified = 1
              mod.swap(curveIndex, kPoint.co.y)
            #end for each keyframe_point
          curveIndex += 1
          #end for each fcurve
        if (modified == 1):
          # key.jointMods.append(mod)
          key.jointMods[mod.jointIndex] = mod
        #end for each action group
      key.numJointMods = len(key.jointMods)
      anim.keyframes.append(key)
      #end for each keyframe
    animations.append(anim)
    #end if this action applies to an object
  #end for each animation

#   Calculate Mesh Data
for my in bpy.data.objects:
  if (my.type == "MESH"):
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
    matCount = 0
    vertIndexCount = 0
    for myMat in my.data.materials:
      faceNum = 0
      mat = wsMaterial(myMat.name)
      for face in my.data.tessfaces:
        if (face.material_index == matCount):
          for v in range( len(face.vertices) ):
            vert = wsVert()
            bVert = my.data.vertices[face.vertices[v]]
            vert.pos = Vector([ bVert.co[0], bVert.co[2], -bVert.co[1] ])
            vert.norm = Vector([ bVert.normal[0], bVert.normal[2], -bVert.normal[1] ])
            if (v == 0):
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv1[0], \
                                my.data.tessface_uv_textures[0].data[faceNum].uv1[1] ]
            elif (v == 1):
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv2[0], \
                                my.data.tessface_uv_textures[0].data[faceNum].uv2[1] ]
            elif (v == 2):
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv3[0], \
                                my.data.tessface_uv_textures[0].data[faceNum].uv3[1] ]
            elif (v == 3):
              vert.texCoords = [ my.data.tessface_uv_textures[0].data[faceNum].uv4[0], \
                                my.data.tessface_uv_textures[0].data[faceNum].uv4[1] ]
            for g in range(len(bVert.groups)):
              vertGroup = my.vertex_groups[bVert.groups[g].group]
              jointid = -1
              boneCount = 0
              foundOne = 0
              for thisBone in animSkel.joints:
                if (thisBone.name == vertGroup.name):
                  jointid = boneCount
                  foundOne = 1
                  break
                boneCount += 1
              if (foundOne == 1):
                vert.weights.append( wsWeight(jointid, bVert.groups[g].weight) )
                vert.numWeights += 1
                vert.weightSum += bVert.groups[g].weight
              #end for each vertex group
            mesh.verts.append(vert)
            #end for each vertex
          mat.tris.append( wsTri(vertIndexCount, vertIndexCount+1, vertIndexCount+2) )
          if (len(face.vertices) == 4):
            mat.tris.append( wsTri(vertIndexCount, vertIndexCount+2, vertIndexCount+3) )
            vertIndexCount += 1
          vertIndexCount += 3
          #end if (this face uses the current materia)l
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
      for tex in myMat.texture_slots:
        if tex != None:
          if tex.use_map_color_diffuse:
            mat.mapBitFlag |= WS_TEXTURE_MAP_COLOR
            mat.colorMap = tex.texture.image.filepath
            fileStart = mat.colorMap.rfind("/") - 1
            mat.colorMap = mat.colorMap[fileStart:]
          elif tex.use_map_normal:
            mat.mapBitFlag |= WS_TEXTURE_MAP_NORMAL
            mat.normalMap = tex.texture.image.filepath
            fileStart = mat.normalMap.rfind("/") - 1
            mat.normalMap = mat.normalMap[fileStart:]
          #end if this texture has been defined
        #end for each texture
      # Check for custom material properties
      for prop in myMat.items():
        if prop[0] != "_RNA_UI":
          mat.properties.append(wsCustomProperty(prop[0], prop[1]))
        # End for each custom property
      mesh.materials.append( mat )
      matCount += 1
      #end for each material
      mesh.numMaterials = len(mesh.materials)
      mesh.numVerts = len(mesh.verts)
    #end for each mesh object

if (mesh != None):
  mesh.numVerts = len(mesh.verts)
  mesh.numMaterials = len(mesh.materials)
  boundsInitialized = 0
  for mat in mesh.materials:
    mat.numTris = len(mat.tris)
  for vert in mesh.verts:
    vert.pos.x *= mesh.scale.x
    vert.pos.y *= mesh.scale.y
    vert.pos.z *= mesh.scale.z
    vert.pos += mesh.location
    vert.pos.rotate(mesh.rotation)
    vert.numWeights = len(vert.weights)
    # Set the default bounding box
    if (boundsInitialized == 0):
      mesh.bounds = wsBounds(vert.pos.x, vert.pos.x, vert.pos.y, vert.pos.y, vert.pos.z, vert.pos.z)
      boundsInitialized = 1
    else:
      mesh.bounds.minX = min(mesh.bounds.minX, vert.pos.x)
      mesh.bounds.maxX = max(mesh.bounds.maxX, vert.pos.x)
      mesh.bounds.minY = min(mesh.bounds.minY, vert.pos.y)
      mesh.bounds.maxY = max(mesh.bounds.maxY, vert.pos.y)
      mesh.bounds.minZ = min(mesh.bounds.minZ, vert.pos.z)
      mesh.bounds.maxZ = max(mesh.bounds.maxZ, vert.pos.z)
    #end for each vertex
  mesh.bounds.halfX = (mesh.bounds.maxX - mesh.bounds.minX) / 2.0
  mesh.bounds.halfY = (mesh.bounds.maxY - mesh.bounds.minY) / 2.0
  mesh.bounds.halfZ = (mesh.bounds.maxZ - mesh.bounds.minZ) / 2.0
  mesh.location.x = (mesh.bounds.maxX + mesh.bounds.minX) / 2.0
  mesh.location.y = (mesh.bounds.maxY + mesh.bounds.minY) / 2.0
  mesh.location.z = (mesh.bounds.maxZ + mesh.bounds.minZ) / 2.0
  #end if we have a mesh

#   ADJUST DATA FOR PARENT OBJECT TRANSFORMATIONS
if (animSkel != None):
  animSkel.numJoints = len(animSkel.joints)
  boundsInitialized = 0
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
    # Set the joint's bounding box
    if (boundsInitialized == 0):
      joint.bounds = wsBounds(min(joint.start.x, joint.end.x), max(joint.start.x, joint.end.x), \
                              min(joint.start.y, joint.end.y), max(joint.start.y, joint.end.y), \
                              min(joint.start.z, joint.end.z), max(joint.start.z, joint.end.z))
      boundsInitialized = 1
    else:
      joint.bounds.minX = min(joint.bounds.minX, joint.start.x, joint.end.x)
      joint.bounds.maxX = max(joint.bounds.maxX, joint.start.x, joint.end.x)
      joint.bounds.minY = min(joint.bounds.minY, joint.start.y, joint.end.y)
      joint.bounds.maxY = max(joint.bounds.maxY, joint.start.y, joint.end.y)
      joint.bounds.minZ = min(joint.bounds.minZ, joint.start.z, joint.end.z)
      joint.bounds.maxZ = max(joint.bounds.maxZ, joint.start.z, joint.end.z)
    #end for each joint
  #end if we have a skeleton
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
      key.jointMods[j].initialRot = key.jointMods[j].rotation
      key.jointMods[j].rotation = animSkel.rotation * animSkel.joints[j].initialRot * key.jointMods[j].initialRot
      key.jointMods[j].location.rotate(key.jointMods[j].rotation)
      par = animSkel.joints[j].parent
      if (par >= 0):
        diffRot = animSkel.joints[par].initialRot.inverted() * animSkel.joints[j].initialRot
        key.jointMods[j].rotation = key.jointMods[par].rotation * diffRot * key.jointMods[j].initialRot
      #end for each jointMod
    #end for each keyframe

  for key in anim.keyframes:
    boundsInitialized = 0
    # Set the keyframe's bounding box
    #apply the animation to a copy.
    jointList = deepcopy(animSkel.joints)
    for j in range(len(jointList)):
      par = animSkel.joints[j].parent
      jointList[j].end -= jointList[j].start
      if (par >= 0):
        jointList[j].startRel = jointList[j].start - animSkel.joints[par].start
        jointList[j].startRel.rotate(animSkel.joints[par].rot.inverted())
      else:
        jointList[j].startRel = jointList[j].start
      jointList[j].end.rotate(jointList[j].rot.inverted())
    for j in range(len(jointList)):
      jointList[j].rot = key.jointMods[j].rotation
      jointList[j].start = jointList[j].startRel
      if (jointList[j].parent >= 0):
        jointList[j].start.rotate(jointList[jointList[j].parent].rot)
        jointList[j].start += jointList[jointList[j].parent].start
      jointList[j].start += key.jointMods[j].location
      jointList[j].end.rotate(key.jointMods[j].rotation)
      jointList[j].end += jointList[j].start
      if (boundsInitialized == 0):
        key.bounds = wsBounds(min(jointList[j].start.x, jointList[j].end.x), max(jointList[j].start.x, jointList[j].end.x), \
                              min(jointList[j].start.y, jointList[j].end.y), max(jointList[j].start.y, jointList[j].end.y), \
                              min(jointList[j].start.z, jointList[j].end.z), max(jointList[j].start.z, jointList[j].end.z))
        boundsInitialized = 1
      else:
        key.bounds.minX = min(key.bounds.minX, jointList[j].start.x, jointList[j].end.x)
        key.bounds.maxX = max(key.bounds.maxX, jointList[j].start.x, jointList[j].end.x)
        key.bounds.minY = min(key.bounds.minY, jointList[j].start.y, jointList[j].end.y)
        key.bounds.maxY = max(key.bounds.maxY, jointList[j].start.y, jointList[j].end.y)
        key.bounds.minZ = min(key.bounds.minZ, jointList[j].start.z, jointList[j].end.z)
        key.bounds.maxZ = max(key.bounds.maxZ, jointList[j].start.z, jointList[j].end.z)
      #end for each joint
    key.bounds.halfX = (key.bounds.maxX - key.bounds.minX) / 2.0
    key.bounds.halfY = (key.bounds.maxY - key.bounds.minY) / 2.0
    key.bounds.halfZ = (key.bounds.maxZ - key.bounds.minZ) / 2.0
    anim.bounds.halfX += key.bounds.halfX
    anim.bounds.halfY += key.bounds.halfY
    anim.bounds.halfZ += key.bounds.halfZ
    #end for each keyframe
  anim.bounds.halfX /= len(anim.keyframes)
  anim.bounds.halfY /= len(anim.keyframes)
  anim.bounds.halfZ /= len(anim.keyframes)
  #end for each animation
        
#   NOW WE WRITE!
if (mesh != None):
  output = wsFileBuffer(mesh.name, "wsMesh")
  output.write("//  Whipstitch Mesh File\n")
  output.write("//  This mesh is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("meshName "+ mesh.name +"\n")
  output.write("numVertices "+ str(mesh.numVerts) +"\n")
  output.write("numMaterials "+ str(mesh.numMaterials) +"\n")
  output.write("defaultPos { %f %f %f }\n" % (mesh.location.x, mesh.location.y, mesh.location.z))
  output.write("hasSkeleton %u\n\n" % hasSkeleton)
  if (hasSkeleton > 0):
    output.write("skeleton {\n")
    skel = mesh.skeleton
    output.write("  numJoints "+ str(skel.numJoints) +"\n") #add one for the root location
    for j in range( skel.numJoints ):
      output.write("  joint "+ str(j) +" {\n")
      output.write("    name "+ skel.joints[j].name+"\n")
      output.write("    parent "+ str(skel.joints[j].parent) +"\n")
      output.write("    pos_start { %f %f %f }\n" % (skel.joints[j].start.x, skel.joints[j].start.y, skel.joints[j].start.z))
      output.write("    pos_end { %f %f %f }\n" % (skel.joints[j].end.x, skel.joints[j].end.y, skel.joints[j].end.z))
      output.write("    rotation { %f %f %f %f }\n" % (skel.joints[j].rot.x, skel.joints[j].rot.y, skel.joints[j].rot.z, \
        skel.joints[j].rot.w))
      output.write("  }\n")
      #end for each joint
    output.write("}\n\n")
    #End if hasSkeleton > 0
  output.write("vertices {\n")
  output.write("  bounds { %f %f %f }\n" % (mesh.bounds.halfX, mesh.bounds.halfY, mesh.bounds.halfZ) )
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
      if (mesh.verts[v].weightSum != 0):
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
    output.write("    ambient { "+ \
      str(mat.ambient[0]) +" "+ \
      str(mat.ambient[1]) +" "+ \
      str(mat.ambient[2]) +" "+ \
      str(mat.ambient[3]) +" }\n")
    output.write("    diffuse { "+ \
      str(mat.diffuse[0]) +" "+ \
      str(mat.diffuse[1]) +" "+ \
      str(mat.diffuse[2]) +" "+ \
      str(mat.diffuse[3]) +" }\n")
    output.write("    specular { "+ \
      str(mat.specular[0]) +" "+ \
      str(mat.specular[1]) +" "+ \
      str(mat.specular[2]) +" "+ \
      str(mat.specular[3]) +" }\n")
    output.write("    emissive { "+ \
      str(mat.emissive[0]) +" "+ \
      str(mat.emissive[1]) +" "+ \
      str(mat.emissive[2]) +" "+ \
      str(mat.emissive[3]) +" }\n")
    output.write("    maps {\n")
    output.write("      bitFlag %u\n" % (mat.mapBitFlag))
    if (mat.colorMap != ""):
      output.write("      colorMap "+ mat.colorMap[2:] +"\n")
    if (mat.normalMap != ""):
      output.write("      normalMap "+ mat.normalMap[2:] +"\n")
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
    output.write("    properties {\n")
    output.write("      numProperties "+ str(len(mat.properties)) +"\n")
    propCount = 0
    for prop in mat.properties:
      output.write("      property "+ str(propCount) +" {\n")
      output.write("        name "+ prop.name +"\n")
      output.write("        value "+ str(prop.value) +"\n")
      output.write("      }\n")
      propCount += 1
      #end for each property
    output.write("    }\n")
    output.write("  }\n")
    #end for each material
  output.write("}\n\n")
  output.apply()
  #end if (mesh exist)s

for a in range( len(animations) ):
  anim = animations[a]
  skel = anim.skeleton
  debug = wsFileBuffer(anim.name, "wsDebug")
  output = wsFileBuffer(anim.name, "wsAnim")
  output.write("//  Whipstitch Animation File\n")
  output.write("//  This Animation is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("animationType 1\n")
  output.write("animationName "+ anim.name +"\n")
  output.write("framesPerSecond "+ str(anim.framesPerSecond) +"\n\n")
  output.write("numJoints "+str(skel.numJoints)+"\n") #add one for the root location
  output.write("numKeyFrames "+ str(anim.numKeyframes) +"\n")
  output.write("bounds { %f %f %f }\n\n" % (anim.bounds.halfX, anim.bounds.halfY, anim.bounds.halfZ) )
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
    debug.write("keyframe %u - bounds { %f %f %f }\n" % (k, key.bounds.halfX, key.bounds.halfY, key.bounds.halfZ))
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
  debug.apply()
  #end for each animation


if (mesh != None):
  output = wsFileBuffer(mesh.name, "wsModel")
  output.write("//  Whipstitch Model File\n")
  output.write("//  This model is for use with the Whipstitch Game Engine\n")
  output.write("//  For more information, email dsnettleton@whipstitchgames.com\n\n")
  output.write("versionNumber "+ str(MAJOR_VERSION) +"."+ str(MINOR_VERSION) +"\n")
  output.write("modelName "+ mesh.name +"\n\n")
  output.write("numMeshes 1\n")
  output.write("numAnimations %d\n" % (len(animations)))

print("Export Complete.")
