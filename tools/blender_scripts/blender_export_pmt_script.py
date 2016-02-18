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

# export one object
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

	vertex_source = "\n" + 3 * "    "
	index_source = "\n" + 3 * "    "
	# write vertex type
	if len(mesh.uv_textures) > 0:
		ET.SubElement(objElem, "VertexType").text="22"
		ET.SubElement(objElem, "VertexCount").text=str(len(mesh.uv_textures.active.data))
	else:
		ET.SubElement(objElem, "VertexType").text="6"
		# write vertex count
		vertex_total_count = len(mesh.vertices)
		ET.SubElement(objElem, "VertexCount").text=str(vertex_total_count)
		# write vertex data
		vertex_current_count = 0
		for vert in mesh.vertices:
			vertex_source += '%.6f, %.6f, %.6f, %.6f, %.6f, %.6f,' % (vert.co.x, vert.co.y, vert.co.z, vert.normal.x, vert.normal.y, vert.normal.z)
			vertex_current_count += 1
			if vertex_current_count < vertex_total_count:
				vertex_source += "\n" + 3 * "    "
		ET.SubElement(objElem, "Vertexes").text=vertex_source + "\n" + 2 * "    "
		# write index count
		index_total_count = 0
		for face in mesh.tessfaces:
			if len(face.vertices) == 3:
				index_total_count += 3
		ET.SubElement(objElem, "IndexCount").text=str(index_total_count)
		# write index data
		index_current_count = 0
		for face in mesh.tessfaces:
			if len(face.vertices) == 3:
				for index in face.vertices:
					index_source = index_source + str(index) + ", "
					index_current_count += 1
					# auto newline if string too long
				if (index_current_count % 12 == 0) and (index_current_count < index_total_count):
					index_source += "\n" + 3 * "    "
		ET.SubElement(objElem, "Indexes").text=index_source + "\n" + 2 * "    "

	# if len(mesh.uv_textures) > 0:
	# 	writeString(file, 'struct vertexDataTextured\n\

	# 	writeString(file, 'static const vertexDataTextured %sVertexData[] = {\n' % basename)
	# 	#for face in uv: #loop through the faces
	# 	uv_layer = mesh.uv_textures.active
	# 	for face in mesh.tessfaces:
	# 		faceUV = uv_layer.data[face.index]
	# 		i=0
	# 		for index in face.vertices:
	# 			if len(face.vertices) == 3:
	# 				vert = mesh.vertices[index]
	# 				writeString(file, '\t{/*v:*/{%f, %f, %f}, ' % (vert.co.x, vert.co.y, vert.co.z) )
	# 				writeString(file, '/*n:*/{%f, %f, %f}, ' % (vert.normal.x, vert.normal.y, vert.normal.z))
	# 				writeString(file, '/*t:*/{%f, %f}' % ( faceUV.uv[i][0], faceUV.uv[i][1] ) )
	# 				writeString(file, '},\n')
	# 				i+=1
	# 	writeString(file, '};\n\n')
	# elif len(mesh.vertex_colors) > 0:
	# 	writeString(file, 'struct vertexDataColored\n\

	# 	writeString(file, 'static const vertexDataColored %sVertexData[] = {\n' % basename)
	# 	color_layer = mesh.vertex_colors.active
	# 	for face in mesh.tessfaces:
	# 		if len(face.vertices) == 3:
	# 			faceC = color_layer.data[face.index]
	# 			i=0
	# 			for index in face.vertices:
	# 				vert = mesh.vertices[index]
	# 				writeString(file, '\t{/*v:*/{%f, %f, %f}, ' % (vert.co.x, vert.co.y, vert.co.z) )
	# 				writeString(file, '/*n:*/{%f, %f, %f}, ' % (vert.normal.x, vert.normal.y, vert.normal.z))
	# 				writeString(file, '/*c:*/{%f, %f, %f, %f}' % ( faceC.color1[i], faceC.color2[i], faceC.color3[i], faceC.color4[i]) )
	# 				writeString(file, '},\n')
	# 				i+=1
	# 	writeString(file, '};\n\n')
	# else:
	# 	writeString(file, 'struct vertexData\n\

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
