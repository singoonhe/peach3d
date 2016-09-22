bl_info = {
    "name": "Export Peach3D Mesh/Skeleton Text (.pmt)",
    "description": "Export Model to Peach3D Mesh/Skeleton Text file",
    "author": "singoon",
    "version": (0, 1),
    "blender": (2, 5, 7),
    "api": 35622,
    "location": "File > Export",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "www.peach3d.com",
    "category": "Import-Export"}

'''
Usage Notes:


'''

import bpy
from bpy.props import *
import mathutils, math, struct
import os
from os import remove
import time
import bpy_extras
from bpy_extras.io_utils import ExportHelper
import time
import shutil
import bpy
import mathutils
import xml.etree.cElementTree as ET
from xml.etree import ElementTree
from xml.dom import minidom

def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ElementTree.tostring(elem, encoding='UTF-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="    ", encoding='UTF-8')

# convert vindex and uv to key
def gen_vertexuv_key(vindex, uv):
    return '%d, %f, %f' % (vindex, round(uv[0], 4), round(uv[1], 4))

# make object triangles if need
def triangulateNMesh(object):
    bneedtri = False
    scene = bpy.context.scene
    bpy.ops.object.mode_set(mode='OBJECT')
    for i in scene.objects: i.select = False #deselect all objects
    object.select = True
    scene.objects.active = object #active current object
    object.data.update(calc_tessface=True)
    for face in object.data.tessfaces:
        if (len(face.vertices) > 3):
            bneedtri = True
            break

    bpy.ops.object.mode_set(mode='OBJECT')
    if bneedtri == True:
        me_ob = object.copy() #copy object
        bpy.context.scene.objects.link(me_ob)#link the object to the scene #current object location
        for i in scene.objects: i.select = False #deselect all objects
        me_ob.select = True
        scene.objects.active = me_ob #set the mesh object to current
        bpy.ops.object.mode_set(mode='EDIT') #Operators
        bpy.ops.mesh.select_all(action='SELECT')#select all the face/vertex/edge
        bpy.ops.mesh.quads_convert_to_tris() #Operators
        bpy.context.scene.update()
        bpy.ops.object.mode_set(mode='OBJECT') # set it in object
        print("Converting quad to triangle mesh done!")
        # remove old object and rename new
        origin_name = object.name
        bpy.context.scene.objects.unlink(object)
        me_ob.name = origin_name
        me_ob.data.update(calc_tessface=True)
    else:
        print("No need to convert triangle mesh.")
        me_ob = object
    return me_ob

def do_calc_rotate_normal(isRot, normal):
    if isRot:
        # if using Y - up, rotate X to reset
        mat_x90 = mathutils.Matrix.Rotation(math.pi/2, 4, 'X')
        return mathutils.Vector(normal) * mat_x90
    else:
        return normal

def add_vertex_weight_string(vertex, mesh):
    # total bones group list
    mesh_groups_list = mesh.vertex_groups
    weight_vector = [0.0, 0.0, 0.0, 0.0]
    for x in range(2):
        is_have_weight = x < len(vertex.groups)
        # store bone weight
        vgroup_weight = is_have_weight and vertex.groups[x].weight or 0.0
        weight_vector[x] = vgroup_weight
        # find bone in pst animation bones list
        if is_have_weight:
            bone_index = vertex.groups[x].group
            weight_vector[x + 2] = bone_index
    return " %f, %f, %.1f, %.1f" % (weight_vector[0], weight_vector[1], weight_vector[2], weight_vector[3])

# export one object, using vertex normal, rendering more smooth.
def do_export_object(context, props, me_ob, xmlRoot):
    # add object
    objElem = ET.SubElement(xmlRoot, "Object", name=me_ob.name)

    # use PREVIEW modifiers
    current_scene = context.scene
    apply_modifiers = props.apply_modifiers
    mesh = me_ob.to_mesh(current_scene, apply_modifiers, 'PREVIEW')
    # is vertex need export weight
    is_export_weight = (len(me_ob.vertex_groups) > 0)

    # is need convert to world space
    if props.world_space:
        mesh.transform(me_ob.matrix_world)
    # set rotate x 90 degree
    if props.rot_x90:
        mat_x90 = mathutils.Matrix.Rotation(-math.pi/2, 4, 'X')
        mesh.transform(mat_x90)

    tail_str = "\n" + 3 * "    "
    vertex_source = tail_str
    index_source = tail_str
    vertex_total_count = 0
    index_total_count = 0
    vertex_type_num = 0
    # write vertex type
    if len(mesh.uv_textures) > 0:
        # VertexType::Point3|VertexType::UV
        vertex_type_num = 34
        if props.export_normal:
            # |VertexType::Normal
            vertex_type_num += 8
        if is_export_weight:
            # |VertexType::Skeleton
            vertex_type_num += 64
        ET.SubElement(objElem, "VertexType").text='%d' % vertex_type_num
        # record vertex and index data
        uv_layer = mesh.uv_layers.active.data
        vert_unique_dict = {}
        for face in me_ob.data.tessfaces:
            if len(face.vertices) == 3:
                vertex_index = face.index * 3
                for index in face.vertices:
                    # calc unique key, charge is vertex existed
                    uv = uv_layer[vertex_index].uv
                    vkey = gen_vertexuv_key(index, uv)
                    vvalue = vert_unique_dict.get(vkey)
                    if vvalue is None:
                        # save current key
                        vert_unique_dict[vkey] = vertex_total_count
                        # record one index data
                        index_source += str(vertex_total_count) + ", "
                        # record one vertex data
                        vert = mesh.vertices[index]
                        if props.export_normal:
                            cal_normal = do_calc_rotate_normal(props.rot_x90, vert.normal)
                            vertex_source += '%f, %f, %f, %f, %f, %f, %f, %f,' % (vert.co.x, vert.co.y, vert.co.z, cal_normal.x, cal_normal.y, cal_normal.z, uv[0], uv[1])
                        else:
                            vertex_source += '%f, %f, %f, %f, %f,' % (vert.co.x, vert.co.y, vert.co.z, uv[0], uv[1])
                        # add bone weight info if need
                        if is_export_weight:
                            vertex_source += add_vertex_weight_string(vert, me_ob)
                        vertex_source += tail_str
                        # vertex data increase once
                        vertex_total_count += 1
                    else:
                        # record one exist index
                        index_source += str(vvalue) + ", "

                    vertex_index += 1
                    # auto newline if string too long
                    index_total_count += 1
                    if (index_total_count % 12 == 0):
                        index_source += tail_str
        # write vertex data and index data
        ET.SubElement(objElem, "VertexCount").text=str(vertex_total_count)
        vertex_source = vertex_source[:-4]
        ET.SubElement(objElem, "Vertexes").text=vertex_source
        ET.SubElement(objElem, "IndexCount").text=str(index_total_count)
        if index_total_count % 12 == 0:
            index_source = index_source[:-4]
        else:
            index_source += tail_str[:-4]
        ET.SubElement(objElem, "Indexes").text=index_source
    else:
        # VertexType::Point3
        vertex_type_num = 2
        if props.export_normal:
            # |VertexType::Normal
            vertex_type_num += 8
        if is_export_weight:
            # |VertexType::Skeleton
            vertex_type_num += 64
        ET.SubElement(objElem, "VertexType").text='%d' % vertex_type_num
        # write vertex count
        vertex_total_count = len(mesh.vertices)
        ET.SubElement(objElem, "VertexCount").text=str(vertex_total_count)
        # write vertex data
        for vert in mesh.vertices:
            if props.export_normal:
                cal_normal = do_calc_rotate_normal(props.rot_x90, vert.normal)
                vertex_source += '%f, %f, %f, %f, %f, %f,' % (vert.co.x, vert.co.y, vert.co.z, cal_normal.x, cal_normal.y, cal_normal.z)
            else:
                vertex_source += '%f, %f, %f,' % (vert.co.x, vert.co.y, vert.co.z)
            # add bone weight info if need
            if is_export_weight:
                vertex_source += add_vertex_weight_string(vert, me_ob)
            vertex_source += tail_str
        vertex_source = vertex_source[:-4]
        ET.SubElement(objElem, "Vertexes").text=vertex_source
        # write index data
        for face in mesh.tessfaces:
            if len(face.vertices) == 3:
                # record index data
                for index in face.vertices:
                    index_source += str(index) + ", "
                    index_total_count += 1
                    # auto newline if string too long
                if (index_total_count % 12 == 0):
                    index_source += tail_str
        ET.SubElement(objElem, "IndexCount").text=str(index_total_count)
        if index_total_count % 12 == 0:
            index_source = index_source[:-4]
        else:
            index_source += tail_str[:-4]
        ET.SubElement(objElem, "Indexes").text=index_source
    print('Export object "%s" , vertexes count %d, vertex type %d' % (me_ob.name, vertex_total_count, vertex_type_num))

    # write bones info, (if need)
    if is_export_weight:
        bonesEle = ET.SubElement(objElem, "Bones")
        for bone in me_ob.vertex_groups:
            ET.SubElement(bonesEle, "Bone").text = bone.name

    # write material info，(take the first one)
    mat_list = mesh.materials
    if len(mat_list) > 0:
        used_mat = mat_list[0]
        matEle = ET.SubElement(objElem, "Material")

        # write ambient color
        world = current_scene.world
        if world:
            world_amb = world.ambient_color
        else:
            world_amb = Color((0.0, 0.0, 0.0))
        ambient = used_mat.ambient * world_amb
        ET.SubElement(matEle, "Ambient").text='%f, %f, %f' % (ambient[0], ambient[1], ambient[2])
        # write diffuse color
        diffuse = used_mat.diffuse_intensity * used_mat.diffuse_color
        ET.SubElement(matEle, "Diffuse").text='%f, %f, %f' % (diffuse[0], diffuse[1], diffuse[2])
        # write specular color
        specular = used_mat.specular_intensity * used_mat.specular_color
        ET.SubElement(matEle, "Specular").text='%f, %f, %f' % (specular[0], specular[1], specular[2])
        # write shininess
        ET.SubElement(matEle, "Shininess").text='%f' % (used_mat.specular_hardness / 2.0)
        # write emissive color
        emissive = used_mat.emit * used_mat.diffuse_color
        ET.SubElement(matEle, "Emissive").text='%f, %f, %f' % (emissive[0], emissive[1], emissive[2])
        # write alpha
        ET.SubElement(matEle, "Alpha").text='%f' % (used_mat.alpha)

        # write texture
        if len(mesh.uv_textures) > 0:
            # write texture file name
            uv_texture = mesh.uv_textures.active.data
            uv_texture_image = uv_texture[0].image
            if uv_texture_image:
                tex_file_path = uv_texture_image.filepath
                texEle = ET.SubElement(matEle, "Texture")
                ET.SubElement(texEle, "File").text=os.path.basename(tex_file_path)
                # default warp uv to TextureWrap::eClampToEdge
                ET.SubElement(texEle, "WrapUV").text='0'
        print('Export material for "%s"' % (me_ob.name))

    return True

#export mesh, maybe have more objects
def do_export_skeleton(context, props, thearmature, filepath):
    context.scene.objects.active    = thearmature
    # return null ifno armature or no data
    if thearmature.animation_data == None:
        print("None actions data Set! skipping...")
        return

    context.scene.frame_set(context.scene.frame_start)
    # find bones invert tranform
    bones_inverted_list = {}
    bpy.ops.object.mode_set(mode='OBJECT')
    total_bones_list = thearmature.data.bones
    for b in total_bones_list:
        transform3 = b.matrix
        translate = b.head
        if b.parent:
            transform3 = b.parent.matrix * transform3
        transform3 = transform3.inverted()
        transform4 = transform3.to_4x4() * mathutils.Matrix.Translation(translate)
        bones_inverted_list[b.name] = transform4

    # write file head
    xmlRoot = ET.Element("Skeleton", version="0.1")
    bpy.ops.object.mode_set(mode='POSE')

    def do_export_bone(bone, xmlParent):
        # convert bone matrix from parent
        if bone.parent != None:
            transform4 = bone.parent.matrix.inverted() * bone.matrix
        else:
            transform4 = bone.matrix
        boneElem = ET.SubElement(xmlParent, "Bone", name=bone.name)
        ET.SubElement(boneElem, "Pose").text = '%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f' % (transform4[0][0],transform4[1][0],transform4[2][0],transform4[3][0],transform4[0][1],transform4[1][1],transform4[2][1],transform4[3][1],transform4[0][2],transform4[1][2],transform4[2][2],transform4[3][2],transform4[0][3],transform4[1][3],transform4[2][3],transform4[3][3])
        for name in bones_inverted_list.keys():
            if name == bone.name:
                transform4 = bones_inverted_list[name]
                ET.SubElement(boneElem, "Inverted").text = '%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f' % (transform4[0][0],transform4[1][0],transform4[2][0],transform4[3][0],transform4[0][1],transform4[1][1],transform4[2][1],transform4[3][1],transform4[0][2],transform4[1][2],transform4[2][2],transform4[3][2],transform4[0][3],transform4[1][3],transform4[2][3],transform4[3][3])
        # add children
        if( bone.children ):
            for child in bone.children:
                do_export_bone(child, boneElem)

    # export all bones
    total_bones_list = thearmature.pose.bones
    for b in total_bones_list:
      if( not b.parent ): #only treat root bones'
        rootBoneNam = b.name
        do_export_bone(b, xmlRoot)
    print('Export root bone "%s", total %d bones' % (rootBoneNam, len(total_bones_list)))

    # export actions
    bpy.ops.object.mode_set(mode='OBJECT')
    anim_rate  = context.scene.render.fps
    restoreAction   = thearmature.animation_data.action    # Q: is animation_data always valid?
    restoreFrame    = context.scene.frame_current       # we already do this in export_proxy, but we'll do it here too for now
    total_bones_list = thearmature.pose.bones
    for arm_action in bpy.data.actions:
        if not len(arm_action.fcurves):
            print("{} has no keys, skipping".format(arm_action.name))
            continue
        # apply action to armature and update scene
        # note if loop all actions that is not armature it will override and will break armature animation.
        # I dont't know why add this code bleow, it will casue object error
        # thearmature.animation_data.action = arm_action
        # context.scene.update()

        # min/max frames define range
        framemin, framemax  = arm_action.frame_range
        start_frame         = int(framemin)
        end_frame           = int(framemax)
        scene_range         = range(start_frame, end_frame + 1)
        frame_count         = len(scene_range)

        # build a list of pose bones relevant to the collated pd_bones_list
        # export bones keyframe
        action_xml_name = arm_action.name
        armEle = ET.SubElement(xmlRoot, "Animation", name=action_xml_name, length='%f' % ((frame_count-1)/anim_rate))
        for i in range(frame_count):
            frame = scene_range[i]
            # advance to frame (automatically updates the pose)
            context.scene.frame_set(frame)

            frameEle = ET.SubElement(armEle, "KeyFrame", time='%f' % (i/anim_rate))
            for pose_bone in total_bones_list:
                pose_bone_matrix = mathutils.Matrix(pose_bone.matrix)
                # convert bone matrix from parent
                if pose_bone.parent != None:
                    pose_bone_parent_matrix = mathutils.Matrix(pose_bone.parent.matrix)
                    pose_bone_matrix        = pose_bone_parent_matrix.inverted() * pose_bone_matrix
                translate = pose_bone_matrix.to_translation()
                rotation  = pose_bone_matrix.to_quaternion().normalized()
                scale  = pose_bone_matrix.to_scale()

                # set x,y,z to negative (rotate in other direction) if have parent
                if pose_bone.parent != None:
                    rotation.x  = -rotation.x
                    rotation.y  = -rotation.y
                    rotation.z  = -rotation.z
                # add bones data
                frameBoneEle = ET.SubElement(frameEle, "Bone", name=pose_bone.name)
                ET.SubElement(frameBoneEle, "Rotation").text = '%f, %f, %f, %f' % (rotation.x, rotation.y, rotation.z, rotation.w)
                ET.SubElement(frameBoneEle, "Scale").text = '%f, %f, %f' % (scale.x, scale.y, scale.z)
                ET.SubElement(frameBoneEle, "Translation").text = '%f, %f, %f' % (translate.x, translate.y, translate.z)
        print('Export animation "%s" from range (%d-%d)' % (action_xml_name, start_frame, end_frame))

    # restore
    thearmature.animation_data.action = restoreAction
    context.scene.frame_set(restoreFrame)

    # save to file
    file = open(filepath, "wb")
    file.write(prettify(xmlRoot))
    file.close()
    return True

#export mesh, maybe have more objects
def do_export_mesh(context, props, meshes, filepath, pstname):
    # write file head
    xmlRoot = ET.Element("Mesh", version="0.1")
    # add skeleton file info
    if pstname:
        ET.SubElement(xmlRoot, "Skeleton").text = pstname

    # write all objects
    for ob in meshes:
        # convert to triangles if need
        me_ob = triangulateNMesh(ob)
        do_export_object(context, props, me_ob, xmlRoot)
    # save to file
    file = open(filepath, "wb")
    file.write(prettify(xmlRoot))
    file.close()
    return True

###### EXPORT OPERATOR #######
class Export_pmt(bpy.types.Operator, ExportHelper):
    '''Exports the active Object as an Peach3D Mesh Text File.'''
    bl_idname = "export_object.pmt"
    bl_label = "Export Peach3D Mesh/Skeleton Text (.pmt)"

    # export file extension
    filename_ext = ".pmt"
    # default export normals
    export_normal = BoolProperty(name="Export Normal",
                            description="Export Normal Datas",
                            default=True)
    # default use PREVIEW modifier
    apply_modifiers = BoolProperty(name="Apply Modifiers",
                            description="Applies the Modifiers",
                            default=True)
    # default rotate 90 degree
    rot_x90 = BoolProperty(name="Convert to Y-up",
                            description="Rotate 90 degrees around X to convert to y-up",
                            default=False)
    # not need transform object to world space
    world_space = BoolProperty(name="Export into Worldspace",
                            description="Transform the Vertexcoordinates into Worldspace",
                            default=False)
    # auto export .pst
    export_pst = BoolProperty(name="Export animation (.pst)",
                            description="Auto export Peach3D Skeleton text file",
                            default=True)


    @classmethod
    def poll(cls, context):
        # mesh must exist
        objValid = False
        for sob in context.scene.objects:
            if sob.type in ['MESH', 'ARMATURE', 'CURVE', 'SURFACE', 'FONT']:
                objValid = True
                break
        return objValid

    def execute(self, context):
        start_time = time.time()
        print('\n_____START_____')
        props = self.properties

        # get full saved filepath
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)
        # find all mesh and armature
        meshes = [obj for obj in bpy.context.scene.objects if obj.type == 'MESH']
        armatures = [arm for arm in bpy.context.scene.objects if arm.type == 'ARMATURE']
        # calc skeleton file name if need
        isNeedPst = len(armatures) > 0 and props.export_pst
        pstname = None
        if isNeedPst:
            pstpath = bpy.path.ensure_ext(filepath, ".pst")
            pstname = os.path.basename(pstpath)
        # export mesh
        exported = do_export_mesh(context, props, meshes, filepath, pstname)
        print('Export mesh file:%s' % filepath)
        # export skekelton
        if isNeedPst:
            exported = do_export_skeleton(context, props, armatures[0], pstpath)
            print('Export skeleton file:%s' % pstpath)

        if exported:
            print('Finished export in %s seconds' %((time.time() - start_time)))
        print('_____END_____\n')

        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager

        if True:
            # File selector
            wm.fileselect_add(self) # will run self.execute()
            return {'RUNNING_MODAL'}
        elif True:
            # search the enum
            wm.invoke_search_popup(self)
            return {'RUNNING_MODAL'}
        elif False:
            # Redo popup
            return wm.invoke_props_popup(self, event) #
        elif False:
            return self.execute(context)

### REGISTER ###

def menu_func(self, context):
    self.layout.operator(Export_pmt.bl_idname, text="Peach3D Mesh/Skeleton Text (.pmt)")

def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
