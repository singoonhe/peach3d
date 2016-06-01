bl_info = {
    "name": "Export Peach3D Skeleton Text (.pst)",
    "description": "Export Animate to Peach3D Skeleton Text file",
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

#export mesh, maybe have more objects
def do_export_skeleton(context, props, filepath):
    bpy.ops.object.mode_set(mode='OBJECT')
    context.scene.frame_set(context.scene.frame_start)
    # write file head
    xmlRoot = ET.Element("Skeleton", version="0.1")

    thearmature = 0  #null to start, will assign in next section
    pd_bones_list = []
    for obj in context.selected_objects:
        if obj.type == 'ARMATURE':
            #skeleton.name = obj.name
            thearmature = obj
            w_matrix = obj.matrix_world

            def do_export_bone(bone, xmlParent, parent = None):
                if (parent and not b.parent.name==parent.name):
                    return #only catch direct children

                # format transform, add to bone
                tsm =  mathutils.Matrix(w_matrix) * mathutils.Matrix(b.matrix_local)  #reversed order of multiplication from 2.4 to 2.5!!! ARRRGGG
                tsmText = ''
                for x in range(4):
                    for y in range(4):
                        tsmText = tsmText + '%.6f' % tsm[x][y]
                        if x != 3 or y != 3:
                            tsmText = tsmText + ', '
                pd_bones_list.append(bone)
                boneElem = ET.SubElement(xmlParent, "Bone", name=bone.name)
                ET.SubElement(boneElem, "Transform").text = tsmText
                # add children
                if( b.children ):
                    for child in b.children: do_export_bone(child, boneElem, bone)

            # export all bones
            for b in thearmature.data.bones:
              if( not b.parent ): #only treat root bones'
                print( "root bone: " + b.name )
                do_export_bone(b, xmlRoot)
            break #only pull one skeleton out

    # return null ifno armature or no data
    if thearmature == 0 or thearmature.animation_data == None:
        print("None Actions Set! skipping...")
        return

    # export actions
    anim_rate  = context.scene.render.fps
    restoreAction   = thearmature.animation_data.action    # Q: is animation_data always valid?
    restoreFrame    = context.scene.frame_current       # we already do this in export_proxy, but we'll do it here too for now
    for arm_action in bpy.data.actions:
        if not len(arm_action.fcurves):
            print("{} has no keys, skipping".format(arm_action.name))
            continue
        # apply action to armature and update scene
        # note if loop all actions that is not armature it will override and will break armature animation.
        thearmature.animation_data.action = arm_action
        context.scene.update()

        # min/max frames define range
        framemin, framemax  = arm_action.frame_range
        start_frame         = int(framemin)
        end_frame           = int(framemax)
        scene_range         = range(start_frame, end_frame + 1)
        frame_count         = len(scene_range)

        # build a list of pose bones relevant to the collated pd_bones_list
        pose_bones_list = []
        for b in pd_bones_list:
            for pb in thearmature.pose.bones:
                if b.name == pb.name:
                    pose_bones_list.append(pb)
                    break;
        # export bones keyframe
        armEle = ET.SubElement(xmlRoot, "Animation", name=arm_action.name, length='%.6f' % ((frame_count-1)/anim_rate))
        for i in range(frame_count):
            frame = scene_range[i]
            # advance to frame (automatically updates the pose)
            context.scene.frame_set(frame)

            frameEle = ET.SubElement(armEle, "KeyFrame", time='%.6f' % (i/anim_rate))
            for pose_bone in pose_bones_list:
                pose_bone_matrix    = mathutils.Matrix(pose_bone.matrix)
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
                ET.SubElement(frameBoneEle, "Rotation").text = '%.6f, %.6f, %.6f, %.6f' % (rotation.x, rotation.y, rotation.z, rotation.w)
                ET.SubElement(frameBoneEle, "Scale").text = '%.6f, %.6f, %.6f' % (scale.x, scale.y, scale.z)
                ET.SubElement(frameBoneEle, "Translation").text = '%.6f, %.6f, %.6f' % (translate.x, translate.y, translate.z)

    # restore
    thearmature.animation_data.action = restoreAction
    context.scene.frame_set(restoreFrame)

    # save to file
    file = open(filepath, "wb")
    file.write(prettify(xmlRoot))
    file.close()


###### EXPORT OPERATOR #######
class Export_pmt(bpy.types.Operator, ExportHelper):
    '''Exports the active Object as an Peach3D Skeleton Text File.'''
    bl_idname = "export_object.pst"
    bl_label = "Export Peach3D Skeleton Text (.pst)"

    # export file extension
    filename_ext = ".pst"

    @classmethod
    def poll(cls, context):
        if len(context.selected_objects) == 0:
            print('No one objects is selected')
            return False
        # every one must vaid
        for sob in context.selected_objects:
            objValid = sob.type in ['ARMATURE']
            if not objValid:
                print('selected objects must all be ARMATURE')
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
        exported = do_export_skeleton(context, props, filepath)

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
    self.layout.operator(Export_pmt.bl_idname, text="Peach3D Skeleton Text (.pst)")

def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
