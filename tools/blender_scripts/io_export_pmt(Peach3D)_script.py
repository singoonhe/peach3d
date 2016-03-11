bl_info = {
    "name": "Export Peach3D Mesh Text (.pmt)",
    "description": "Export Model to Peach3D Mesh Text file",
    "author": "singoon",
    "version": (0, 1),
    "blender": (2, 5, 7),
    "api": 35622,
    "location": "File > Export",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
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
    scene.objects.active = object #set the mesh object to current
    print("Checking mesh if needs to convert quad to Tri...")
    object.data.update(calc_tessface=True)
    for face in object.data.tessfaces:
        if (len(face.vertices) > 3):
            bneedtri = True
            break

    bpy.ops.object.mode_set(mode='OBJECT')
    if bneedtri == True:
        print("Converting quad to tri mesh...")
        me_da = object.data.copy() #copy data
        me_ob = object.copy() #copy object
        #note two copy two types else it will use the current data or mesh
        me_ob.data = me_da
        bpy.context.scene.objects.link(me_ob)#link the object to the scene #current object location
        for i in scene.objects: i.select = False #deselect all objects
        me_ob.select = True
        scene.objects.active = me_ob #set the mesh object to current
        bpy.ops.object.mode_set(mode='EDIT') #Operators
        bpy.ops.mesh.select_all(action='SELECT')#select all the face/vertex/edge
        bpy.ops.mesh.quads_convert_to_tris() #Operators
        bpy.context.scene.update()
        bpy.ops.object.mode_set(mode='OBJECT') # set it in object
        print("Triangulate Mesh Done!")
        print("Remove Merge tmp Mesh [ " ,object.name, " ] from scene!" )
        bpy.ops.object.mode_set(mode='OBJECT') # set it in object
        bpy.context.scene.objects.unlink(object)
    else:
        print("No need to convert tri mesh.")
        me_ob = object
    return me_ob

def do_calc_rotate_normal(isRot, normal):
    if isRot:
        # if using Y - up, rotate X to reset
        mat_x90 = mathutils.Matrix.Rotation(math.pi/2, 4, 'X')
        return mathutils.Vector(normal) * mat_x90
    else:
        return normal

# export one object, using vertex normal, rendering more smooth.
def do_export_object(context, props, me_ob, xmlRoot, isLast):
    # add object
    objElem = ET.SubElement(xmlRoot, "Object", name=me_ob.name)

    # use PREVIEW modifiers
    current_scene = context.scene
    apply_modifiers = props.apply_modifiers
    mesh = me_ob.to_mesh(current_scene, apply_modifiers, 'PREVIEW')

    # is need convert to world space
    if props.world_space:
        mesh.transform(ob.matrix_world)
    # set rotate x 90 degree
    if props.rot_x90:
        mat_x90 = mathutils.Matrix.Rotation(-math.pi/2, 4, 'X')
        mesh.transform(mat_x90)

    tail_str = "\n" + 3 * "    "
    vertex_source = tail_str
    index_source = tail_str
    # write vertex type
    if len(mesh.uv_textures) > 0:
        if props.export_normal:
            # VertexType::Point3|VertexType::Normal|VertexType::UV
            ET.SubElement(objElem, "VertexType").text="22"
        else:
            # VertexType::Point3|VertexType::UV
            ET.SubElement(objElem, "VertexType").text="18"
        # record vertex and index data
        uv_layer = mesh.uv_layers.active.data
        index_current_count = 0
        vert_unique_count = 0
        vert_unique_dict = {}
        for face in mesh.tessfaces:
            if len(face.vertices) == 3:
                vertex_index = face.index * 3
                for index in face.vertices:
                    # calc unique key, charge is vertex existed
                    uv = uv_layer[vertex_index].uv
                    vkey = gen_vertexuv_key(index, uv)
                    vvalue = vert_unique_dict.get(vkey)
                    if vvalue is None:
                        # save current key
                        vert_unique_dict[vkey] = vert_unique_count
                        # record one index data
                        index_source += str(vert_unique_count) + ", "
                        # record one vertex data
                        vert = mesh.vertices[index]
                        if props.export_normal:
                            cal_normal = do_calc_rotate_normal(props.rot_x90, vert.normal)
                            vertex_source += '%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f,' % (vert.co.x, vert.co.y, vert.co.z, cal_normal.x, cal_normal.y, cal_normal.z, uv[0], 1.0 - uv[1])
                        else:
                            vertex_source += '%.6f, %.6f, %.6f, %.6f, %.6f,' % (vert.co.x, vert.co.y, vert.co.z, uv[0], 1.0 - uv[1])
                        vertex_source += tail_str
                        # vertex data increase once
                        vert_unique_count += 1
                    else:
                        # record one exist index
                        index_source += str(vvalue) + ", "

                    vertex_index += 1
                    # auto newline if string too long
                    index_current_count += 1
                    if (index_current_count % 12 == 0):
                        index_source += tail_str
        # write vertex data and index data
        ET.SubElement(objElem, "VertexCount").text=str(vert_unique_count)
        vertex_source = vertex_source[:-4]
        ET.SubElement(objElem, "Vertexes").text=vertex_source
        ET.SubElement(objElem, "IndexCount").text=str(index_current_count)
        if index_current_count % 12 == 0:
            index_source = index_source[:-4]
        else:
            index_source += tail_str[:-4]
        ET.SubElement(objElem, "Indexes").text=index_source
    else:
        if props.export_normal:
            # VertexType::Point3|VertexType::Normal
            ET.SubElement(objElem, "VertexType").text="6"
        else:
            # VertexType::Point3
            ET.SubElement(objElem, "VertexType").text="2"
        # write vertex count
        vertex_total_count = len(mesh.vertices)
        ET.SubElement(objElem, "VertexCount").text=str(vertex_total_count)
        # write vertex data
        for vert in mesh.vertices:
            if props.export_normal:
                cal_normal = do_calc_rotate_normal(props.rot_x90, vert.normal)
                vertex_source += '%.6f, %.6f, %.6f, %.6f, %.6f, %.6f,' % (vert.co.x, vert.co.y, vert.co.z, cal_normal.x, cal_normal.y, cal_normal.z)
            else:
                vertex_source += '%.6f, %.6f, %.6f,' % (vert.co.x, vert.co.y, vert.co.z)
            vertex_source += tail_str
        vertex_source = vertex_source[:-4]
        ET.SubElement(objElem, "Vertexes").text=vertex_source
        # write index data
        index_current_count = 0
        for face in mesh.tessfaces:
            if len(face.vertices) == 3:
                # record index data
                for index in face.vertices:
                    index_source += str(index) + ", "
                    index_current_count += 1
                    # auto newline if string too long
                if (index_current_count % 12 == 0):
                    index_source += tail_str
        ET.SubElement(objElem, "IndexCount").text=str(index_current_count)
        if index_current_count % 12 == 0:
            index_source = index_source[:-4]
        else:
            index_source += tail_str[:-4]
        ET.SubElement(objElem, "Indexes").text=index_source

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
        ET.SubElement(matEle, "Ambient").text='%.6f, %.6f, %.6f' % (ambient[0], ambient[1], ambient[2])
        # write diffuse color
        diffuse = used_mat.diffuse_intensity * used_mat.diffuse_color
        ET.SubElement(matEle, "Diffuse").text='%.6f, %.6f, %.6f' % (diffuse[0], diffuse[1], diffuse[2])
        # write specular color
        specular = used_mat.specular_intensity * used_mat.specular_color
        ET.SubElement(matEle, "Specular").text='%.6f, %.6f, %.6f' % (specular[0], specular[1], specular[2])
        # write shininess
        ET.SubElement(matEle, "Shininess").text='%.6f' % (used_mat.specular_hardness / 2.0)
        # write emissive color
        emissive = used_mat.emit * used_mat.diffuse_color
        ET.SubElement(matEle, "Emissive").text='%.6f, %.6f, %.6f' % (emissive[0], emissive[1], emissive[2])
        # write alpha
        ET.SubElement(matEle, "Alpha").text='%.6f' % (used_mat.alpha)

        # write texture
        if len(mesh.uv_textures) > 0:
            texEle = ET.SubElement(matEle, "Texture")
            # write texture file name
            uv_texture = mesh.uv_textures.active.data
            tex_file_path = uv_texture[0].image.filepath
            ET.SubElement(texEle, "File").text=os.path.basename(tex_file_path)
            # default warp uv to TextureWrap::eClampToEdge
            ET.SubElement(texEle, "WrapUV").text='0'

    return True

#export mesh, maybe have more objects
def do_export_mesh(context, props, filepath):
    # write file head
    xmlRoot = ET.Element("Mesh", version="0.1")

    # write all selected objects
    ob_list = context.selected_objects
    obj_count = len(ob_list)
    obj_index = 1
    for ob in ob_list:
        # convert to triangles if need
        me_ob = triangulateNMesh(ob)
        do_export_object(context, props, me_ob, xmlRoot, obj_index==obj_count)
        obj_index = obj_index + 1
    # save to file
    file = open(filepath, "wb")
    file.write(prettify(xmlRoot))
    file.close()


###### EXPORT OPERATOR #######
class Export_pmt(bpy.types.Operator, ExportHelper):
    '''Exports the active Object as an Peach3D Mesh Text File.'''
    bl_idname = "export_object.pmt"
    bl_label = "Export Peach3D Mesh Text (.pmt)"

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
                            default=True)
    # not need transform object to world space
    world_space = BoolProperty(name="Export into Worldspace",
                            description="Transform the Vertexcoordinates into Worldspace",
                            default=False)


    @classmethod
    def poll(cls, context):
        if len(context.selected_objects) == 0:
            print('No one objects is selected')
            return False
        # every one must vaid
        for sob in context.selected_objects:
            objValid = sob.type in ['MESH', 'CURVE', 'SURFACE', 'FONT']
            if not objValid:
                print('selected objects must all be MESH/CURVE/SURFACE/FONT')
                return False
        return True

    def execute(self, context):
        start_time = time.time()
        print('\n_____START_____')
        props = self.properties

        # get full saved filepath
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)
        # export file with setting properties
        exported = do_export_mesh(context, props, filepath)

        if exported:
            print('finished export in %s seconds' %((time.time() - start_time)))
            print(filepath)

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
    self.layout.operator(Export_pmt.bl_idname, text="Peach3D Mesh Text (.pmt)")

def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
