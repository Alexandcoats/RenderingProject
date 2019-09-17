import json
import bpy

objs = sorted(bpy.context.scene.objects, key=lambda o: o.name)
            
final = {}
            
for o in objs:
    if o.name.startswith('tile'):
        tile = o.name[:5]
        subMesh = o.name[6:] if len(o.name) > 5 else ''
        if not tile in final:
            final[o.name] = {'location': [v for v in o.location], 
                             'rotation': [v for v in o.rotation_euler],
                             'scale': [v for v in o.scale],
                             'subMeshes': {},
                             'lights': []}
        elif o.type == 'MESH':
            final[tile]['subMeshes'][subMesh] = {'location': [v for v in o.location], 
                                                 'rotation': [v for v in o.rotation_euler],
                                                 'scale': [v for v in o.scale]}
        elif o.type == 'LIGHT':
            final[tile]['lights'].append({'pos': [v for v in o.location], 
                                          'color': [v for v in o.data.color], 
                                          'brightness': o.data.energy})
    
with open('C:/Users/acoats/source/repos/RenderingProject/RenderingProject/models/metadata.json', 'w') as f:
    f.write(json.dumps(final, indent=4))