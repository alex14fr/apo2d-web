
# Perform the top-down filtration of Apogee data
# Remove blocks accessible only via suspended items
# Prepare uplinks and their cardinality if applied 
def prepare_graph(data):
    rv=[]
    for level in data:
        blocks={}
        uplinks=[]
        active_elps={}
        for x in level:
            parent=x['parent']
            # If the parent is not active, skip the list
            if parent and parent not in rv[-1]['active_elps']:
                continue
            l=x['list']
            code=l['code']
            # If a new list is found, add it to this level
            if code not in blocks:
                b={'list':{'code':code, 'type':l['type']}, 'items':x['items']}
                blocks[code]=b
                # Add active elps on this level
                for i, item in enumerate(x['items']):
                    if 'suspended' not in item:
                        code_elp=item['code']
                        if code_elp not in active_elps:
                            active_elps[code_elp]=[]
                        active_elps[code_elp].append({'list':code, 'pos':i})
            # Add an uplink (whether the list is new or already seen):
            if parent:
                uplinks.append({'parent':parent, 'child': code, 'min':l['min'], 'max':l['max']})
        rv.append({'blocks':blocks, 'uplinks':uplinks, 'active_elps':active_elps})
    return rv


