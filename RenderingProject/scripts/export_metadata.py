import json
import bpy

objs = sorted(bpy.context.scene.objects, key=lambda o: o.name)
objs = [o for o in objs if o.name.startswith('tile')]

objs = [{'name': o.name, 'location': [v for v in o.location]} for o in objs]
    
with open('C:/Users/acoats/source/repos/RenderingProject/RenderingProject/models/metadata.json', 'w') as f:
    f.write(json.dumps(objs))